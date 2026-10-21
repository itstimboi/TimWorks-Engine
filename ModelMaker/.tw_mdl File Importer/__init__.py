import bpy

from . import exporter
from . import importer


def menu_func_export(self, context):
    self.layout.operator(
        exporter.TIMWORKS_OT_export.bl_idname,
        text="TimWorks Model (.tw_mdl)"
    )
    self.layout.operator(
        importer.TIMWORKS_OT_export.bl_idname,
        text="TimWorks Model (.tw_mdl)"
    )


def register():
    exporter.register()
    importer.register()

    bpy.types.TOPBAR_MT_file_export.append(
        menu_func_export
    )


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(
        menu_func_export
    )

    exporter.unregister()
    importer.unregister()


if __name__ == "__main__":
    register()