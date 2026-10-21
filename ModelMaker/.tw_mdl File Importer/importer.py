
import bpy
import json
import base64
import os
import tempfile
import math
from mathutils import Matrix, Vector
from bpy_extras.io_utils import ImportHelper
from bpy.props import BoolProperty
from bpy.types import Operator


# ------------------------------------------------------------
# Matrix and validation helpers
# ------------------------------------------------------------

def list_to_matrix(values):
    """Convert a row-major, 16-float TW matrix to a Blender Matrix."""
    if not isinstance(values, (list, tuple)) or len(values) != 16:
        raise ValueError("Expected a matrix containing 16 numbers.")

    return Matrix([
        [float(values[row * 4 + col]) for col in range(4)]
        for row in range(4)
    ])


def get_collection(context):
    """Use the active collection, falling back to the scene collection."""
    if context.collection is not None:
        return context.collection
    return context.scene.collection


def safe_name(value, fallback):
    return str(value) if value else fallback


def validate_model(model):
    if not isinstance(model, dict):
        raise ValueError("The file root must be a JSON object.")

    if "idleFrame" not in model:
        raise ValueError("Missing required 'idleFrame' data.")

    frame = model["idleFrame"]
    if not isinstance(frame, dict):
        raise ValueError("'idleFrame' must be an object.")

    vertices = frame.get("vertices", [])
    indices = frame.get("indices", [])

    if not isinstance(vertices, list) or not isinstance(indices, list):
        raise ValueError("Vertices and indices must be arrays.")

    if len(indices) % 3 != 0:
        raise ValueError("The mesh index count must be divisible by 3.")

    for index in indices:
        if not isinstance(index, int) or index < 0 or index >= len(vertices):
            raise ValueError(f"Invalid mesh vertex index: {index}")

    return model


# ------------------------------------------------------------
# Embedded textures
# ------------------------------------------------------------

def load_embedded_texture(texture_data, material_name):
    """Decode a Base64 image, load it through Blender, and pack it."""
    if not isinstance(texture_data, dict):
        return None

    if not texture_data.get("embedded", False):
        return None

    encoded = texture_data.get("data", "")
    if not encoded:
        return None

    mime = texture_data.get("mime", "image/png").lower()
    extensions = {
        "image/png": ".png",
        "image/jpeg": ".jpg",
        "image/jpg": ".jpg",
        "image/tiff": ".tif",
        "image/webp": ".webp",
    }
    extension = extensions.get(mime)

    if extension is None:
        print(f"TimWorks: Unsupported embedded texture type: {mime}")
        return None

    try:
        image_bytes = base64.b64decode(encoded, validate=True)
    except Exception as exc:
        print(f"TimWorks: Could not decode texture for {material_name}: {exc}")
        return None

    if not image_bytes:
        return None

    image = None
    temp_path = None

    try:
        with tempfile.NamedTemporaryFile(
            suffix=extension, delete=False
        ) as temp_file:
            temp_file.write(image_bytes)
            temp_path = temp_file.name

        image = bpy.data.images.load(temp_path, check_existing=False)
        image.name = f"{material_name}_Texture"
        image.pack()
        return image

    except Exception as exc:
        if image is not None:
            try:
                bpy.data.images.remove(image)
            except Exception:
                pass

        print(f"TimWorks: Could not load texture for {material_name}: {exc}")
        return None

    finally:
        if temp_path and os.path.exists(temp_path):
            try:
                os.remove(temp_path)
            except OSError:
                pass


# ------------------------------------------------------------
# Materials
# ------------------------------------------------------------

def create_materials(model):
    materials = []

    for index, material_data in enumerate(model.get("materials", [])):
        name = safe_name(
            material_data.get("name"),
            f"TW_Material_{index}"
        )

        material = bpy.data.materials.new(name=name)
        material.use_nodes = True

        nodes = material.node_tree.nodes
        links = material.node_tree.links

        bsdf = nodes.get("Principled BSDF")
        output = nodes.get("Material Output")

        if bsdf is not None and output is not None:
            if not any(
                link.from_node == bsdf and link.to_node == output
                for link in links
            ):
                links.new(bsdf.outputs["BSDF"], output.inputs["Surface"])

        texture = load_embedded_texture(
            material_data.get("texture"),
            name
        )

        if texture is not None and bsdf is not None:
            image_node = nodes.new("ShaderNodeTexImage")
            image_node.image = texture
            image_node.location = (-400, 200)

            links.new(
                image_node.outputs["Color"],
                bsdf.inputs["Base Color"]
            )

            alpha_input = bsdf.inputs.get("Alpha")
            if alpha_input is not None:
                links.new(image_node.outputs["Alpha"], alpha_input)

            material.diffuse_color = (1.0, 1.0, 1.0, 1.0)

        materials.append(material)

    return materials


# ------------------------------------------------------------
# Mesh construction
# ------------------------------------------------------------

def get_vertex_position(vertex):
    position = vertex.get("position", [0, 0, 0])
    if len(position) != 3:
        raise ValueError("A vertex position must contain three numbers.")
    return tuple(float(value) for value in position)


def get_vertex_normal(vertex):
    normal = vertex.get("normal", [0, 1, 0])
    if len(normal) != 3:
        return (0.0, 1.0, 0.0)
    return tuple(float(value) for value in normal)


def get_vertex_color(vertex):
    color = vertex.get("color", [255, 255, 255, 255])

    if len(color) == 3:
        color = list(color) + [255]

    # The C++ format uses raylib Color channels, which are 0..255.
    if max(color, default=0) > 1:
        return tuple(max(0.0, min(1.0, float(c) / 255.0)) for c in color)

    return tuple(max(0.0, min(1.0, float(c))) for c in color)


def get_vertex_uv(vertex):
    uv = vertex.get("texcoord", [0, 0])
    if len(uv) != 2:
        return (0.0, 0.0)
    return tuple(float(value) for value in uv)


def build_faces(indices):
    return [
        (indices[i], indices[i + 1], indices[i + 2])
        for i in range(0, len(indices), 3)
    ]


def assign_surface_materials(mesh, surfaces):
    """Map each triangle's first index to its TW surface."""
    polygon_count = len(mesh.polygons)

    for surface in surfaces:
        first_index = int(surface.get("firstIndex", 0))
        index_count = int(surface.get("indexCount", 0))
        material_index = int(surface.get("material", -1))

        if first_index < 0 or index_count < 0:
            continue

        first_triangle = first_index // 3
        last_triangle = min(
            (first_index + index_count) // 3,
            polygon_count
        )

        if first_index % 3 != 0 or index_count % 3 != 0:
            print("TimWorks: Surface index range is not triangle-aligned.")

        for polygon_index in range(first_triangle, last_triangle):
            polygon = mesh.polygons[polygon_index]

            if 0 <= material_index < len(mesh.materials):
                polygon.material_index = material_index

            shading = surface.get("shading", "Smooth")
            polygon.use_smooth = str(shading).lower() == "smooth"


def import_mesh_data(frame, object_name, materials, collection):
    vertices_data = frame.get("vertices", [])
    indices = frame.get("indices", [])
    surfaces = frame.get("surfaces", [])

    positions = [get_vertex_position(v) for v in vertices_data]
    faces = build_faces(indices)

    mesh = bpy.data.meshes.new(f"{object_name}_Mesh")
    mesh.from_pydata(positions, [], faces)
    mesh.update()

    obj = bpy.data.objects.new(object_name, mesh)
    collection.objects.link(obj)

    for material in materials:
        mesh.materials.append(material)

    assign_surface_materials(mesh, surfaces)

    # UVs are stored per TW vertex. Duplicate vertices across material
    # surfaces preserve seams produced by the exporter.
    if vertices_data and mesh.loops:
        uv_layer = mesh.uv_layers.new(name="UVMap")
        for loop in mesh.loops:
            uv_layer.data[loop.index].uv = get_vertex_uv(
                vertices_data[loop.vertex_index]
            )

    # Vertex colors are stored as a CORNER attribute for broad compatibility.
    if vertices_data and mesh.loops:
        try:
            color_attr = mesh.color_attributes.new(
                name="Color",
                type="FLOAT_COLOR",
                domain="CORNER"
            )
            for loop in mesh.loops:
                color_attr.data[loop.index].color = get_vertex_color(
                    vertices_data[loop.vertex_index]
                )
        except Exception as exc:
            print(f"TimWorks: Could not import vertex colors: {exc}")

    # Preserve the supplied normals when Blender supports custom split normals.
    if vertices_data and mesh.loops:
        try:
            normals = [
                get_vertex_normal(vertices_data[loop.vertex_index])
                for loop in mesh.loops
            ]
            mesh.normals_split_custom_set(normals)
        except Exception as exc:
            # Newer Blender versions may manage custom normals differently.
            print(f"TimWorks: Using Blender-calculated normals: {exc}")

    return obj


# ------------------------------------------------------------
# Armature and weights
# ------------------------------------------------------------

def resolve_bone_matrices(bones):
    """
    TW bone transforms are local-to-parent matrices.
    Convert them to armature-space matrices, even if the list is not ordered.
    """
    resolved = {}
    visiting = set()

    def resolve(index):
        if index in resolved:
            return resolved[index]

        if index < 0 or index >= len(bones):
            raise ValueError(f"Invalid bone index: {index}")

        if index in visiting:
            raise ValueError("Bone hierarchy contains a parent cycle.")

        visiting.add(index)
        bone = bones[index]
        local = list_to_matrix(bone.get("transform", [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]))

        parent_index = int(bone.get("parent", -1))

        if parent_index < 0:
            world = local
        else:
            world = resolve(parent_index) @ local

        visiting.remove(index)
        resolved[index] = world
        return world

    for i in range(len(bones)):
        resolve(i)

    return resolved


def create_armature(frame, collection, mesh_obj):
    bones = frame.get("bones", [])
    if not bones:
        return None, {}

    armature_data = bpy.data.armatures.new(f"{mesh_obj.name}_Armature")
    armature_obj = bpy.data.objects.new(
        f"{mesh_obj.name}_Armature",
        armature_data
    )
    collection.objects.link(armature_obj)

    # Keep the armature and mesh in the same object space.
    armature_obj.matrix_world = mesh_obj.matrix_world.copy()

    bpy.context.view_layer.objects.active = armature_obj
    armature_obj.select_set(True)

    bpy.ops.object.mode_set(mode="EDIT")

    edit_bones = []
    bone_indices = {}

    for index, bone_data in enumerate(bones):
        name = safe_name(bone_data.get("name"), f"Bone_{index}")
        edit_bone = armature_data.edit_bones.new(name)
        edit_bone.length = 0.25
        edit_bones.append(edit_bone)
        bone_indices[name] = index

    matrices = resolve_bone_matrices(bones)

    for index, bone_data in enumerate(bones):
        edit_bone = edit_bones[index]
        edit_bone.matrix = matrices[index]

        parent_index = int(bone_data.get("parent", -1))
        if 0 <= parent_index < len(edit_bones) and parent_index != index:
            edit_bone.parent = edit_bones[parent_index]
            edit_bone.use_connect = False

        # The file has no explicit bone-length field. Keep a visible,
        # non-zero length while retaining the imported head and orientation.
        edit_bone.length = max(edit_bone.length, 0.05)

    # Save names while the edit-bone collection is still valid.
    imported_bone_names = [bone.name for bone in edit_bones]

    bpy.ops.object.mode_set(mode="OBJECT")
    armature_obj.select_set(False)

    modifier = mesh_obj.modifiers.new("TimWorks Armature", "ARMATURE")
    modifier.object = armature_obj

    # Vertex groups use bone indices, as stored in boneIds.
    groups = []
    for group_name in imported_bone_names:
        groups.append(mesh_obj.vertex_groups.new(name=group_name))

    for vertex_index, vertex in enumerate(frame.get("vertices", [])):
        bone_ids = vertex.get("boneIds", [0, 0, 0, 0])
        weights = vertex.get("weights", [1, 0, 0, 0])

        for slot in range(min(len(bone_ids), len(weights), 4)):
            bone_index = int(bone_ids[slot])
            weight = float(weights[slot])

            if weight <= 0 or not 0 <= bone_index < len(groups):
                continue

            groups[bone_index].add([vertex_index], weight, "REPLACE")

    return armature_obj, bone_indices


# ------------------------------------------------------------
# Skeletal animation import
# ------------------------------------------------------------

def set_pose_from_tw_frame(armature_obj, animation_frame, bone_names):
    """
    Apply one sampled pose. Each TW transform is local-to-parent.
    PoseBone.matrix expects armature-space transforms.
    """
    frame_bones = animation_frame.get("bones", [])
    if not frame_bones:
        return

    matrices = {}
    by_name = {}

    for bone_data in frame_bones:
        name = bone_data.get("name")
        if name:
            by_name[name] = bone_data

    # Use the armature's original bone order for parent evaluation.
    ordered_names = list(bone_names.keys())
    visiting = set()

    def resolve_pose(name):
        if name in matrices:
            return matrices[name]

        if name in visiting:
            raise ValueError("Animation contains a bone hierarchy cycle.")

        bone_data = by_name.get(name)
        pose_bone = armature_obj.pose.bones.get(name)

        if bone_data is None or pose_bone is None:
            return None

        visiting.add(name)
        local = list_to_matrix(bone_data.get("transform", [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]))

        parent_name = pose_bone.parent.name if pose_bone.parent else None
        parent_matrix = resolve_pose(parent_name) if parent_name else None

        world = (parent_matrix @ local) if parent_matrix is not None else local
        visiting.remove(name)
        matrices[name] = world
        return world

    # Compute matrices first, then apply them in hierarchy order.
    for name in ordered_names:
        resolve_pose(name)

    for name, matrix in matrices.items():
        pose_bone = armature_obj.pose.bones.get(name)
        if pose_bone is not None:
            pose_bone.matrix = matrix


def import_animations(model, armature_obj, scene):
    animations = model.get("animations", [])
    frames = model.get("frames", [])

    if armature_obj is None or not animations or not frames:
        return 0

    fps = scene.render.fps / max(scene.render.fps_base, 0.001)
    bone_names = {
        bone.name: index
        for index, bone in enumerate(armature_obj.data.bones)
    }

    animation_data = armature_obj.animation_data_create()
    original_action = animation_data.action
    original_frame = scene.frame_current
    original_subframe = scene.frame_subframe
    created_count = 0

    try:
        for animation in animations:
            animation_name = safe_name(
                animation.get("name"),
                f"TW_Animation_{created_count}"
            )
            keys = animation.get("keys", [])
            if not keys:
                continue

            # Start a separate action for each TW animation.
            animation_data.action = None
            action = None

            for key in keys:
                frame_index = int(key.get("frame", -1))
                if not 0 <= frame_index < len(frames):
                    print(
                        f"TimWorks: Skipping invalid frame index "
                        f"{frame_index} in {animation_name}"
                    )
                    continue

                seconds = max(0.0, float(key.get("time", 0.0)))
                blender_frame = max(1, int(round(seconds * fps)) + 1)

                scene.frame_set(blender_frame)
                set_pose_from_tw_frame(
                    armature_obj,
                    frames[frame_index],
                    bone_names
                )

                for pose_bone in armature_obj.pose.bones:
                    pose_bone.rotation_mode = "QUATERNION"
                    pose_bone.keyframe_insert(
                        data_path="location",
                        frame=blender_frame,
                        group=pose_bone.name
                    )
                    pose_bone.keyframe_insert(
                        data_path="rotation_quaternion",
                        frame=blender_frame,
                        group=pose_bone.name
                    )
                    pose_bone.keyframe_insert(
                        data_path="scale",
                        frame=blender_frame,
                        group=pose_bone.name
                    )

                if animation_data.action is not None:
                    action = animation_data.action

            if action is None:
                action = animation_data.action

            if action is not None:
                action.name = animation_name

                # Use linear interpolation only when requested by the TW key.
                for key in keys:
                    frame_index = int(key.get("frame", -1))
                    if not 0 <= frame_index < len(frames):
                        continue

                    seconds = max(0.0, float(key.get("time", 0.0)))
                    blender_frame = max(1, int(round(seconds * fps)) + 1)
                    interpolation = "LINEAR" if key.get("lerp", True) else "CONSTANT"

                    try:
                        for fcurve in action.fcurves:
                            for point in fcurve.keyframe_points:
                                if abs(point.co.x - blender_frame) < 0.001:
                                    point.interpolation = interpolation
                    except (AttributeError, TypeError):
                        # Blender versions using layered Actions may expose
                        # curves through slots rather than action.fcurves.
                        pass

                created_count += 1

    finally:
        animation_data.action = original_action
        scene.frame_set(original_frame, subframe=original_subframe)
        bpy.context.view_layer.update()

    return created_count


# ------------------------------------------------------------
# Collision mesh
# ------------------------------------------------------------

def import_collision(model, collection, object_name):
    collision = model.get("collision", {})
    vertices = collision.get("vertices", [])
    indices = collision.get("indices", [])

    if not vertices or not indices:
        return None

    positions = []
    for position in vertices:
        if len(position) != 3:
            raise ValueError("Collision vertex must have three coordinates.")
        positions.append(tuple(float(v) for v in position))

    for index in indices:
        if not isinstance(index, int) or not 0 <= index < len(positions):
            raise ValueError(f"Invalid collision vertex index: {index}")

    faces = build_faces(indices)

    mesh = bpy.data.meshes.new(f"{object_name}_CollisionMesh")
    mesh.from_pydata(positions, [], faces)
    mesh.update()

    obj = bpy.data.objects.new(f"{object_name}_Collision", mesh)
    collection.objects.link(obj)
    obj.display_type = "WIRE"
    obj.hide_render = True

    return obj


# ------------------------------------------------------------
# Main import function
# ------------------------------------------------------------

def import_tw_model(context, filepath, import_animations=True,
                    import_collision_mesh=True):
    with open(filepath, "r", encoding="utf-8") as file:
        model = validate_model(json.load(file))

    collection = get_collection(context)
    object_name = os.path.splitext(os.path.basename(filepath))[0]

    materials = create_materials(model)
    mesh_obj = import_mesh_data(
        model["idleFrame"],
        object_name,
        materials,
        collection
    )

    armature_obj, _ = create_armature(
        model["idleFrame"],
        collection,
        mesh_obj
    )

    animation_count = 0
    if import_animations and armature_obj is not None:
        animation_count = import_animations_fn(
            model,
            armature_obj,
            context.scene
        )

    collision_obj = None
    if import_collision_mesh:
        collision_obj = import_collision(
            model,
            collection,
            object_name
        )

    # Select the primary mesh after import.
    bpy.ops.object.select_all(action="DESELECT")
    mesh_obj.select_set(True)
    context.view_layer.objects.active = mesh_obj

    return mesh_obj, armature_obj, animation_count, collision_obj


# Avoid a name collision between the argument and the function.
import_animations_fn = import_animations


# ------------------------------------------------------------
# Blender file-browser operator
# ------------------------------------------------------------

class TIMWORKS_OT_import(Operator, ImportHelper):
    bl_idname = "import_scene.timworks_model"
    bl_label = "Import TimWorks Model"
    bl_description = "Import a .tw_mdl model into Blender"
    bl_options = {"REGISTER", "UNDO"}

    filename_ext = ".tw_mdl"

    filter_glob: bpy.props.StringProperty(
        default="*.tw_mdl",
        options={"HIDDEN"}
    )

    import_animations: BoolProperty(
        name="Import Animations",
        description="Rebuild skeletal animations as Blender actions",
        default=True
    )

    import_collision_mesh: BoolProperty(
        name="Import Collision Mesh",
        description="Create a separate wireframe collision object",
        default=True
    )

    def execute(self, context):
        try:
            mesh_obj, armature_obj, animation_count, collision_obj = (
                import_tw_model(
                    context,
                    self.filepath,
                    self.import_animations,
                    self.import_collision_mesh
                )
            )

            message = f"Imported {mesh_obj.name}"
            if armature_obj:
                message += f", armature: {armature_obj.name}"
            if animation_count:
                message += f", animations: {animation_count}"
            if collision_obj:
                message += ", collision mesh"

            self.report({"INFO"}, message)
            return {"FINISHED"}

        except Exception as exc:
            self.report({"ERROR"}, f"TimWorks import failed: {exc}")
            print("TimWorks importer traceback:")
            import traceback
            traceback.print_exc()
            return {"CANCELLED"}


def menu_func_import(self, context):
    self.layout.operator(
        TIMWORKS_OT_import.bl_idname,
        text="TimWorks Model (.tw_mdl)"
    )


def register():
    bpy.utils.register_class(TIMWORKS_OT_import)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.utils.unregister_class(TIMWORKS_OT_import)
