import bpy
import json
import base64
import os

from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty
from bpy.types import Operator


# ============================================================
# Helpers
# ============================================================

def clamp_byte(value):
    return int(max(0.0, min(1.0, value)) * 255)


def get_active_uv_layer(mesh):
    if hasattr(mesh, "uv_layers"):
        return mesh.uv_layers.active

    return None


def get_vertex_color(mesh, loop_index):
    # Blender 3.x / 4.x / 5.x color attributes
    if hasattr(mesh, "color_attributes"):
        attributes = mesh.color_attributes

        if attributes:
            attribute = attributes.active

            if attribute:
                try:
                    color = attribute.data[loop_index].color

                    return [
                        clamp_byte(color[0]),
                        clamp_byte(color[1]),
                        clamp_byte(color[2]),
                        clamp_byte(color[3])
                    ]

                except Exception:
                    pass

    # Older Blender versions
    if hasattr(mesh, "vertex_colors"):
        layers = mesh.vertex_colors

        if layers:
            layer = layers.active

            if layer:
                try:
                    color = layer.data[loop_index].color

                    return [
                        clamp_byte(color[0]),
                        clamp_byte(color[1]),
                        clamp_byte(color[2]),
                        clamp_byte(color[3])
                    ]

                except Exception:
                    pass

    return [255, 255, 255, 255]


# ============================================================
# Texture Export
# ============================================================

def find_material_image(material):
    """
    Find an image connected to the Principled BSDF Base Color.

    Returns:
        bpy.types.Image or None
    """

    if material is None:
        return None

    if not material.use_nodes:
        return None

    nodes = material.node_tree.nodes
    links = material.node_tree.links

    principled = None

    # Find Principled BSDF
    for node in nodes:
        if node.type == "BSDF_PRINCIPLED":
            principled = node
            break

    if principled is None:
        return None

    base_color_input = principled.inputs.get("Base Color")

    if base_color_input is None:
        return None

    # Look for a linked Image Texture
    for link in links:
        if link.to_node == principled:
            if link.to_socket == base_color_input:
                source = link.from_node

                if source.type == "TEX_IMAGE":
                    return source.image

    return None


def image_to_png_base64(image):
    """
    Convert a Blender image into PNG bytes and Base64.

    The original Blender image is not modified.
    """

    if image is None:
        return ""

    # Save the image to a temporary PNG file.
    temp_dir = bpy.app.tempdir

    if not temp_dir:
        temp_dir = os.path.dirname(bpy.data.filepath)

    if not temp_dir:
        temp_dir = "."

    temp_path = os.path.join(
        temp_dir,
        "timworks_texture_export.png"
    )

    old_filepath = image.filepath_raw
    old_format = image.file_format

    try:
        image.filepath_raw = temp_path
        image.file_format = "PNG"

        image.save()

        with open(temp_path, "rb") as file:
            data = file.read()

        return base64.b64encode(data).decode("ascii")

    finally:
        image.filepath_raw = old_filepath
        image.file_format = old_format

        try:
            if os.path.exists(temp_path):
                os.remove(temp_path)
        except Exception:
            pass


# ============================================================
# Materials
# ============================================================

def export_materials(objects):
    materials = []
    material_indices = {}

    for obj in objects:
        for slot in obj.material_slots:

            material = slot.material

            if material is None:
                continue

            name = material.name

            if name in material_indices:
                continue

            index = len(materials)

            material_indices[name] = index

            texture = {
                "embedded": False,
                "mime": "",
                "data": ""
            }

            # Try to find a Base Color image
            image = find_material_image(material)

            if image is not None:
                try:
                    base64_data = image_to_png_base64(image)

                    if base64_data:
                        texture["embedded"] = True
                        texture["mime"] = "image/png"
                        texture["data"] = base64_data

                except Exception as error:
                    print(
                        "[TimWorks] Failed to export texture:",
                        error
                    )

            materials.append({
                "name": name,
                "texture": texture
            })

    return materials, material_indices


import math


def get_animated_bone_transform(pose_bone):
    """
    Get an animated bone's local transform relative to its
    parent pose bone, using Blender's evaluated pose.
    """

    if pose_bone.parent is not None:
        local_matrix = (
            pose_bone.parent.matrix.inverted()
            @ pose_bone.matrix
        )
    else:
        local_matrix = pose_bone.matrix.copy()

    return matrix_to_list(local_matrix)


def assign_action(armature, action):
    """
    Assign an action to an armature, including an action slot
    when the Blender version supports layered actions.
    """

    if armature.animation_data is None:
        armature.animation_data_create()

    animation_data = armature.animation_data
    animation_data.action = action

    # Blender versions with layered Actions use action slots.
    if hasattr(action, "slots"):
        slots = action.slots

        if len(slots) > 0 and hasattr(
            animation_data, "action_slot"
        ):
            animation_data.action_slot = slots[0]

    return animation_data

def get_action_keyframe_times(action, animation_data):
    """
    Return the unique frame times containing actual keyframes.

    Supports legacy Actions and layered Actions in newer Blender
    versions. Only reads the active Action slot when applicable.
    """

    keyframe_times = set()

    def collect_fcurves(fcurves):
        for fcurve in fcurves:
            for keyframe in fcurve.keyframe_points:
                keyframe_times.add(float(keyframe.co.x))

    # Legacy Actions, used by older Blender versions.
    if hasattr(action, "fcurves"):
        try:
            collect_fcurves(action.fcurves)
        except (AttributeError, TypeError):
            pass

    # Layered Actions, used by newer Blender versions.
    if hasattr(action, "layers"):
        slot = getattr(animation_data, "action_slot", None)

        if slot is not None:
            for layer in action.layers:
                for strip in layer.strips:
                    try:
                        channelbag = strip.channelbag(slot)
                    except (AttributeError, TypeError, RuntimeError):
                        channelbag = None

                    if channelbag is not None:
                        collect_fcurves(channelbag.fcurves)

    return sorted(keyframe_times)


def export_animations(context, armature, bone_indices):
    """
    Export animations using sparse TRS bone transforms.

    Each animation frame contains only bones whose transforms
    changed since the previous sampled keyframe.

    Animated bone schema:
        {
            "name": "Bone",
            "parent": 0,
            "location": [x, y, z],
            "rotation": [w, x, y, z],
            "scale": [x, y, z]
        }

    The importer must retain omitted bones' previous transforms.
    """

    frames = []
    animations = []

    if armature is None:
        return frames, animations

    scene = context.scene
    view_layer = context.view_layer

    original_frame = scene.frame_current
    original_subframe = scene.frame_subframe

    if armature.animation_data is None:
        armature.animation_data_create()

    animation_data = armature.animation_data

    original_action = animation_data.action
    original_slot = getattr(
        animation_data, "action_slot", None
    )

    fps = scene.render.fps / scene.render.fps_base

    if fps <= 0:
        fps = 24.0

    try:
        for action in bpy.data.actions:

            # Assign the action and a supported action slot.
            assign_action(armature, action)

            keyframe_times = get_action_keyframe_times(
                action,
                animation_data
            )

            if not keyframe_times:
                continue

            start_time = keyframe_times[0]

            keys = []

            # Remember the previous sampled transform for each bone.
            # Reset this for every separate animation.
            previous_transforms = {}

            for blender_time in keyframe_times:

                whole_frame = math.floor(blender_time)
                subframe = blender_time - whole_frame

                scene.frame_set(
                    whole_frame,
                    subframe=subframe
                )

                view_layer.update()

                animated_bones = []

                for bone in armature.data.bones:

                    pose_bone = armature.pose.bones.get(
                        bone.name
                    )

                    if pose_bone is None:
                        continue

                    parent_index = (
                        bone_indices.get(
                            bone.parent.name, -1
                        )
                        if bone.parent is not None
                        else -1
                    )

                    # Convert the evaluated pose into compact TRS.
                    matrix = get_animated_bone_matrix(
                        pose_bone
                    )

                    transform = matrix_to_trs(matrix)

                    previous = previous_transforms.get(
                        bone.name
                    )

                    # Skip this bone if its pose did not change.
                    if transforms_equal(transform, previous):
                        continue

                    animated_bones.append({
                        "name": bone.name,
                        "parent": parent_index,
                        **transform
                    })

                    previous_transforms[bone.name] = transform

                # Mesh geometry is stored once in idleFrame.
                model_frame = {
                    "vertices": [],
                    "indices": [],
                    "bones": animated_bones,
                    "surfaces": []
                }

                model_frame_index = len(frames)
                frames.append(model_frame)

                elapsed_seconds = (
                    blender_time - start_time
                ) / fps

                keys.append({
                    "frame": model_frame_index,
                    "time": float(elapsed_seconds),
                    "lerp": True
                })

            animations.append({
                "name": action.name,
                "keys": keys
            })

    finally:
        # Restore the original action and slot.
        animation_data.action = original_action

        if hasattr(animation_data, "action_slot"):
            animation_data.action_slot = original_slot

        scene.frame_set(
            original_frame,
            subframe=original_subframe
        )

        view_layer.update()

    return frames, animations


def get_vertex_bone_weights(
    obj,
    vertex,
    bone_indices
):
    """
    Get up to four bone influences for a Blender vertex.

    Blender stores weights through vertex groups. Each vertex group
    whose name matches an exported bone is treated as a bone
    influence.
    """

    influences = []

    for group in vertex.groups:

        group_index = group.group

        if group_index < 0:
            continue

        if group_index >= len(obj.vertex_groups):
            continue

        vertex_group = obj.vertex_groups[group_index]

        bone_name = vertex_group.name

        if bone_name not in bone_indices:
            continue

        weight = float(
            vertex_group.weight(
                vertex.index
            )
        )

        if weight <= 0.0:
            continue

        influences.append(
            (
                bone_indices[bone_name],
                weight
            )
        )

    # Strongest influences first.
    influences.sort(
        key=lambda influence: influence[1],
        reverse=True
    )

    # TimWorks supports exactly four influences.
    influences = influences[:4]

    # Normalize the remaining weights.
    total_weight = sum(
        weight
        for _, weight in influences
    )

    if total_weight > 0.0:

        influences = [
            (
                bone_id,
                weight / total_weight
            )
            for bone_id, weight in influences
        ]

    bone_ids = [0, 0, 0, 0]

    weights = [1.0, 0.0, 0.0, 0.0]

    for index, (bone_id, weight) in enumerate(
        influences
    ):

        bone_ids[index] = int(bone_id)
        weights[index] = float(weight)

    # If the vertex has no bone weights, keep it attached
    # to bone 0 if a skeleton exists.
    #
    # Otherwise it remains the default [0, 0, 0, 0].
    if not influences and bone_indices:

        bone_ids = [0, 0, 0, 0]
        weights = [1.0, 0.0, 0.0, 0.0]

    return bone_ids, weights

def get_animated_bone_matrix(pose_bone):
    """Return the bone's local pose matrix."""

    if pose_bone.parent is not None:
        return (
            pose_bone.parent.matrix.inverted()
            @ pose_bone.matrix
        )

    return pose_bone.matrix.copy()


def matrix_to_trs(matrix):
    """
    Convert a matrix to translation, quaternion rotation,
    and scale. Returns JSON-compatible lists.
    """

    location, rotation, scale = matrix.decompose()

    return {
        "location": [
            round(float(location.x), 6),
            round(float(location.y), 6),
            round(float(location.z), 6)
        ],
        "rotation": [
            round(float(rotation.w), 6),
            round(float(rotation.x), 6),
            round(float(rotation.y), 6),
            round(float(rotation.z), 6)
        ],
        "scale": [
            round(float(scale.x), 6),
            round(float(scale.y), 6),
            round(float(scale.z), 6)
        ]
    }


def transforms_equal(a, b, tolerance=0.00001):
    """Return True when two TRS transforms are equivalent."""

    if b is None:
        return False

    # Compare location and scale normally.
    for component in ("location", "scale"):
        if any(
            abs(x - y) > tolerance
            for x, y in zip(a[component], b[component])
        ):
            return False

    # q and -q represent the same rotation.
    qa = a["rotation"]
    qb = b["rotation"]

    direct_error = max(
        abs(x - y) for x, y in zip(qa, qb)
    )

    inverse_error = max(
        abs(x + y) for x, y in zip(qa, qb)
    )

    return min(direct_error, inverse_error) <= tolerance


def matrix_to_list(matrix):
    """
    Convert a Blender 4x4 matrix into a flat list of 16 floats.

    Blender matrices are column-oriented internally, but this
    preserves the matrix values in Blender's normal row/column
    indexing order.
    """

    return [
        float(matrix[row][column])
        for row in range(4)
        for column in range(4)
    ]


def find_armature(objects):
    """
    Find an armature associated with the exported mesh objects.

    First checks Armature modifiers, then falls back to any
    selected armature.
    """

    for obj in objects:

        if obj.type != "MESH":
            continue

        for modifier in obj.modifiers:

            if modifier.type == "ARMATURE":

                armature = modifier.object

                if armature is not None:
                    return armature

    for obj in objects:

        if obj.type == "ARMATURE":
            return obj

    return None


def export_bones(objects):
    """
    Export the armature bones.

    The transform stored in TWModel is the bone's local transform
    relative to its parent.
    """

    armature = find_armature(objects)

    if armature is None:
        return [], {}

    armature_data = armature.data

    # ------------------------------------------------------------
    # Create the bone index table first.
    # ------------------------------------------------------------

    bone_indices = {}

    for index, bone in enumerate(
        armature_data.bones
    ):

        bone_indices[bone.name] = index

    # ------------------------------------------------------------
    # Export the bones.
    # ------------------------------------------------------------

    bones = []

    for bone in armature_data.bones:

        if bone.parent is not None:

            parent_index = bone_indices.get(
                bone.parent.name,
                -1
            )

            local_matrix = (
                bone.parent.matrix_local.inverted()
                @ bone.matrix_local
            )

        else:

            parent_index = -1

            local_matrix = bone.matrix_local.copy()

        bones.append({
            "name": bone.name,

            "parent": parent_index,

            "transform": matrix_to_list(
                local_matrix
            )
        })

    return bones, bone_indices

# ============================================================
# Mesh Export
# ============================================================

def export_mesh(objects, material_indices, depsgraph, bones, bone_indices):
    vertices = []
    indices = []
    surfaces = []

    # ------------------------------------------------------------
    # Export one object at a time.
    #
    # Each object gets its triangles grouped by material.
    # This guarantees every surface occupies one contiguous
    # section of the index buffer.
    # ------------------------------------------------------------

    for obj in objects:

        if obj.type != "MESH":
            continue

        evaluated_obj = obj.evaluated_get(depsgraph)
        mesh = evaluated_obj.to_mesh()

        if mesh is None:
            continue

        try:
            world_matrix = evaluated_obj.matrix_world

            mesh.calc_loop_triangles()

            uv_layer = get_active_uv_layer(mesh)

            # ----------------------------------------------------
            # Build one triangle list for each exported material.
            #
            # {
            #     material_index: [
            #         (loop1, loop2, loop3),
            #         ...
            #     ]
            # }
            # ----------------------------------------------------

            material_triangles = {}

            for triangle in mesh.loop_triangles:

                blender_material_index = (
                    triangle.material_index
                )

                exported_material = -1

                if (
                    blender_material_index >= 0
                    and
                    blender_material_index <
                    len(evaluated_obj.material_slots)
                ):
                    slot = evaluated_obj.material_slots[
                        blender_material_index
                    ]

                    if slot.material:
                        exported_material = (
                            material_indices.get(
                                slot.material.name,
                                -1
                            )
                        )

                if exported_material not in material_triangles:
                    material_triangles[
                        exported_material
                    ] = []

                material_triangles[
                    exported_material
                ].append(triangle)

            # ----------------------------------------------------
            # Export each material group.
            # ----------------------------------------------------

            for exported_material, triangles in (
                material_triangles.items()
            ):

                first_index = len(indices)

                # A separate vertex map for each object is enough,
                # but the key still contains all attributes that
                # can make two vertex instances different.
                vertex_map = {}

                for triangle in triangles:

                    for loop_index in triangle.loops:

                        loop = mesh.loops[loop_index]

                        blender_vertex = mesh.vertices[
                            loop.vertex_index
                        ]
                        
                        bone_ids, weights = get_vertex_bone_weights(
                            obj,
                            blender_vertex,
                            bone_indices
                        )

                        # ------------------------------------------------
                        # Position
                        # ------------------------------------------------

                        position = (
                            world_matrix @ blender_vertex.co
                        )

                        # ------------------------------------------------
                        # Normal
                        # ------------------------------------------------

                        normal = loop.normal.copy()

                        normal = (
                            world_matrix.to_3x3()
                            .inverted()
                            .transposed()
                            @ normal
                        )

                        normal.normalize()

                        # ------------------------------------------------
                        # UV
                        # ------------------------------------------------

                        if uv_layer:
                            uv = uv_layer.data[
                                loop_index
                            ].uv

                            texcoord = [
                                float(uv.x),
                                float(uv.y)
                            ]
                        else:
                            texcoord = [
                                0.0,
                                0.0
                            ]

                        # ------------------------------------------------
                        # Vertex color
                        # ------------------------------------------------

                        color = get_vertex_color(
                            mesh,
                            loop_index
                        )

                        # ------------------------------------------------
                        # Vertex key
                        # ------------------------------------------------

                        key = (
                            loop.vertex_index,

                            round(position.x, 7),
                            round(position.y, 7),
                            round(position.z, 7),

                            round(normal.x, 7),
                            round(normal.y, 7),
                            round(normal.z, 7),

                            round(texcoord[0], 7),
                            round(texcoord[1], 7),

                            tuple(color)
                        )

                        # ------------------------------------------------
                        # Create vertex
                        # ------------------------------------------------

                        if key not in vertex_map:

                            vertex_index = len(vertices)

                            vertex_map[key] = (
                                vertex_index
                            )

                            vertices.append({
                                "position": [
                                    float(position.x),
                                    float(position.y),
                                    float(position.z)
                                ],

                                "normal": [
                                    float(normal.x),
                                    float(normal.y),
                                    float(normal.z)
                                ],

                                "color": color,

                                "texcoord": texcoord,

                                "boneIds": bone_ids,

                                "weights": weights
                            })

                        indices.append(
                            vertex_map[key]
                        )

                # ------------------------------------------------
                # Calculate exactly how many indices this surface
                # added.
                # ------------------------------------------------

                index_count = (
                    len(indices) - first_index
                )

                # ------------------------------------------------
                # Create surface.
                # ------------------------------------------------

                surfaces.append({
                    "firstIndex": first_index,

                    "indexCount": index_count,

                    "material": exported_material,

                    "shading": "Smooth"
                })

        finally:
            evaluated_obj.to_mesh_clear()

    return {
        "vertices": vertices,
        "indices": indices,
        "bones": bones,
        "surfaces": surfaces
    }

# ============================================================
# Model Creation
# ============================================================


def create_tw_model(context):

    objects = [
        obj
        for obj in context.selected_objects
        if obj.type == "MESH"
    ]

    if not objects:
        raise RuntimeError(
            "No mesh objects are selected."
        )

    depsgraph = context.evaluated_depsgraph_get()

    # Export the static mesh and its materials.
    materials, material_indices = export_materials(
        objects
    )

    bones, bone_indices = export_bones(objects)

    idle_frame = export_mesh(
        objects,
        material_indices,
        depsgraph,
        bones,
        bone_indices
    )

    # Find the armature through the mesh's Armature modifier.
    armature = find_armature(objects)

    frames, animations = export_animations(
        context,
        armature,
        bone_indices
    )

    return {
        "version": 0.2,

        "animationFormat": "sparse-trs-v1",

        "materials": materials,

        "idleFrame": idle_frame,

        "collision": {
            "vertices": [],
            "indices": [],
            "surfaces": []
        },

        "frames": frames,

        "animations": animations
    }

# import json
# import base64

def report_model_sizes(model):
    print("\n===== TimWorks Model Size Report =====")

    def size_of(value):
        return len(json.dumps(value, separators=(",", ":")).encode("utf-8"))

    print(f"Materials:   {size_of(model['materials']):,} bytes")
    print(f"Idle mesh:   {size_of(model['idleFrame']):,} bytes")
    print(f"Animations:  {size_of(model['animations']):,} bytes")
    print(f"Frames:      {size_of(model['frames']):,} bytes")

    total_vertices = len(model["idleFrame"]["vertices"])
    total_indices = len(model["idleFrame"]["indices"])
    total_frames = len(model["frames"])
    total_bones = len(model["idleFrame"]["bones"])

    print(f"Vertices:    {total_vertices:,}")
    print(f"Indices:     {total_indices:,}")
    print(f"Frames:      {total_frames:,}")
    print(f"Bones:       {total_bones:,}")

    texture_bytes = 0

    for material in model["materials"]:
        texture = material.get("texture", {})
        data = texture.get("data", "")

        if data:
            texture_bytes += len(base64.b64decode(data))

    print(f"Embedded PNGs: {texture_bytes:,} bytes")
    print("======================================\n")

# ============================================================
# Export Operator
# ============================================================

class TIMWORKS_OT_export(
    Operator,
    ExportHelper
):

    bl_idname = "export_scene.timworks_model"

    bl_label = "Export TimWorks Model"

    bl_options = {"PRESET"}

    filename_ext = ".tw_mdl"

    filter_glob: StringProperty(
        default="*.tw_mdl",
        options={"HIDDEN"}
    )

    def execute(self, context):

        try:

            model = create_tw_model(
                context
            )

            with open(
                self.filepath,
                "w",
                encoding="utf-8"
            ) as file:

                report_model_sizes(model)

                json.dump(
                    model,
                    file,
                    separators=(",", ":"),
                    ensure_ascii=False
                )

            self.report(
                {"INFO"},
                "TimWorks model exported successfully"
            )

            return {"FINISHED"}

        except Exception as error:

            self.report(
                {"ERROR"},
                str(error)
            )

            return {"CANCELLED"}


# ============================================================
# Registration
# ============================================================

classes = (
    TIMWORKS_OT_export,
)


def register():

    for cls in classes:
        bpy.utils.register_class(cls)


def unregister():

    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)