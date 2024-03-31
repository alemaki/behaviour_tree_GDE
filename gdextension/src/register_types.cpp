/* godot-cpp integration testing project.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include "behaviour_tree/behaviour_tree.hpp"
#include "behaviour_tree/tasks/bt_action.hpp"
#include "behaviour_tree/tasks/bt_composite.hpp"
#include "behaviour_tree/tasks/bt_condition.hpp"
#include "behaviour_tree/tasks/bt_decorator.hpp"
#include "behaviour_tree/tasks/bt_subtree.hpp"
#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/composites/bt_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_random_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_random_selector.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_fail.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_succeed.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"
#include "behaviour_tree/tasks/decorators/bt_probability.hpp"
#include "behaviour_tree/ui/bt_editor_plugin.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"
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
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
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
		ClassDB::register_class<BTSelector>();
		ClassDB::register_class<BTSequence>();
		ClassDB::register_class<BTRandomSequence>();
		ClassDB::register_class<BTRandomSelector>();
		ClassDB::register_class<BTCondition>();
		ClassDB::register_class<BTAlwaysSucceed>();
		ClassDB::register_class<BTAlwaysFail>();
		ClassDB::register_class<BTInvert>();
		ClassDB::register_class<BTProbability>();
		ClassDB::register_class<BTRepeat>();
		ClassDB::register_class<BTAction>();

		ClassDB::register_class<BTSubtree>();
		ClassDB::register_class<BehaviourTree>();

		/* Run doctest tests */
		doctest::Context context;
		context.applyCommandLine(0, nullptr);
		int res = context.run();

		if (context.shouldExit())
		{
			return;
		}
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		
		ClassDB::register_internal_class<BTGraphNode>();
		ClassDB::register_internal_class<BTGraphNodeSubtree>();
		ClassDB::register_internal_class<BTGraphEditor>();
		ClassDB::register_internal_class<BTEditorPlugin>();
		
		EditorPlugins::add_by_type<BTEditorPlugin>();
	}
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
