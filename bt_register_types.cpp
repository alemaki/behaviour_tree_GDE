/* godot-cpp integration testing project.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "behaviour/conditions/bt_check_value.hpp"
#include "behaviour_tree/behaviour_tree.hpp"
#include "behaviour_tree/tasks/bt_action.hpp"
#include "behaviour_tree/tasks/bt_composite.hpp"
#include "behaviour_tree/tasks/bt_condition.hpp"
#include "behaviour_tree/tasks/bt_decorator.hpp"
#include "behaviour_tree/tasks/bt_subtree.hpp"
#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/composites/bt_dynamic_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_dynamic_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_parallel.hpp"
#include "behaviour_tree/tasks/composites/bt_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_random_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_random_selector.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_run.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_fail.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_succeed.hpp"
#include "behaviour_tree/tasks/decorators/bt_cooldown.hpp"
#include "behaviour_tree/tasks/decorators/bt_delay.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"
#include "behaviour_tree/tasks/decorators/bt_probability.hpp"
#include "behaviour_tree/ui/bt_editor_debugger_plugin.hpp"
#include "behaviour_tree/ui/bt_editor_plugin.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"
#include "behaviour_tree/ui/bt_graph_sort_algorithm.hpp"
#include "behaviour_tree/ui/bt_graph_view.hpp"
#include "blackboard/blackboard.hpp"
#include "finite_state_machine/fsm.hpp"
#include "finite_state_machine/state.hpp"

#include "tests/test_utils/test_runner.hpp"


using namespace godot;

void initialize_behaviour_tree_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<Blackboard>();

		ClassDB::register_class<BTTask>();
		ClassDB::register_class<BTDecorator>();
		ClassDB::register_class<BTComposite>();
		ClassDB::register_class<BTParallel>();
		ClassDB::register_class<BTDynamicSelector>();
		ClassDB::register_class<BTDynamicSequence>();
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
		ClassDB::register_class<BTCooldown>();
		ClassDB::register_class<BTDelay>();
		ClassDB::register_class<BTAction>();

		ClassDB::register_class<BehaviourTree>();

		ClassDB::register_class<BTSubtree>();

		ClassDB::register_class<State>();
		ClassDB::register_class<FSM>();


		ClassDB::register_class<BTCheckValue>();
		
		ClassDB::register_internal_class<BTAlwaysRun>();
		
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		ClassDB::register_internal_class<BTGraphNode>();
		ClassDB::register_internal_class<BTGraphNodeSubtree>();
		ClassDB::register_internal_class<BTGraphSortAlgorithm>();
		ClassDB::register_internal_class<BTGraphView>();
		ClassDB::register_internal_class<BTGraphEditor>();
		ClassDB::register_internal_class<BTEditorPlugin>();
		ClassDB::register_internal_class<BTEditorDebuggerPlugin>();

		EditorPlugins::add_by_type<BTEditorPlugin>();
	}
}

void initialize_test_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<TestRunner>();
	}
}

void uninitialize_behaviour_tree_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
}
