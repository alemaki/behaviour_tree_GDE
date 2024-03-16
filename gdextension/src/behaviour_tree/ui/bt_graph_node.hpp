#ifndef BT_GRAPH_NODE_HPP
#define BT_GRAPH_NODE_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/graph_node.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/composites/bt_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_random_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_random_sequence.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_fail.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_succeed.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"
#include "behaviour_tree/tasks/decorators/bt_probability.hpp"
#include "behaviour_tree/tasks/decorators/bt_repeat.hpp"
#include "behaviour_tree/tasks/bt_action.hpp"

class BTGraphNode : public godot::GraphNode
{
    GDCLASS(BTGraphNode, godot::GraphNode)

private:
    godot::Label* task_type_label;
    godot::GraphEdit* graph_edit;
    godot::Ref<BTTask> task;

private:
    void _on_gui_input(const godot::Ref<godot::InputEvent>& event);
    void _setup_connections_ui();
    void _setup_task_type_label();
public:
    BTGraphNode();
    void set_graph_edit(godot::GraphEdit* graph_edit);
    _FORCE_INLINE_ godot::GraphEdit* get_graph_editor() const
    {
        return this->graph_edit;
    }

    void set_task(godot::Ref<BTTask> task);

    _FORCE_INLINE_ godot::Ref<BTTask> get_task() const
    {
        return this->task;
    }

protected:
    static void _bind_methods();
};



#endif /* BT_GRAPH_NODE_HPP */