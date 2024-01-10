/* godot-cpp integration testing project.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "test.hpp"

#include "behaviour_tree/tasks/bt_action.hpp"
#include "behaviour_tree/tasks/bt_composite.hpp"
#include "behaviour_tree/tasks/bt_condition.hpp"
#include "behaviour_tree/tasks/bt_decorator.hpp"
#include "behaviour_tree/tasks/bt_task.hpp"
#include "components/area2d/hitbox.hpp"
#include "components/area2d/hurtbox.hpp"
#include "components/health/health_component.hpp"
#include "components/input/character_input_component.hpp"
#include "components/movement/character_movement_component.hpp"
#include "projectiles/projectile.hpp"
#include "projectiles/linear_projectile.hpp"


using namespace godot;

void initialize_gdextension_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
	ClassDB::register_class<Test>();
	ClassDB::register_class<Projectile>();
	ClassDB::register_class<HealthComponent>();
	ClassDB::register_class<Hitbox>();
	ClassDB::register_class<Hurtbox>();
	ClassDB::register_class<LinearProjectile>();
	ClassDB::register_class<CharacterInputComponent>();
	ClassDB::register_class<CharacterMovementComponent>();
	ClassDB::register_class<BTTask>();
	ClassDB::register_class<BTDecorator>();
	ClassDB::register_class<BTComposite>();
	ClassDB::register_class<BTCondition>();
	ClassDB::register_class<BTAction>();
}

void uninitialize_gdextension_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
}

extern "C"
{
// Initialization.
GDExtensionBool GDE_EXPORT gdextension_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
{
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_gdextension_module);
	init_obj.register_terminator(uninitialize_gdextension_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
