#include "register_types.h"
#include "overlay.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_overlay_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ClassDB::register_class<Overlay>();
    }
}

void uninitialize_overlay_module(ModuleInitializationLevel p_level) {
    // Cleanup if needed
}

extern "C" {
GDExtensionBool GDE_EXPORT overlay_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_overlay_module);
    init_obj.register_terminator(uninitialize_overlay_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}