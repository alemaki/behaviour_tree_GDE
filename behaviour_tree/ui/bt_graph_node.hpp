#ifndef BT_GRAPH_NODE_HPP
#define BT_GRAPH_NODE_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/graph_node.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/label.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"

class BTGraphNode : public godot::GraphNode
{
    GDCLASS(BTGraphNode, godot::GraphNode)

private:
    godot::GraphEdit* graph_edit;

protected:
    godot::Label* task_type_label;
    godot::Ref<BTTask> task;

private:
    void _on_gui_input(const godot::Ref<godot::InputEvent>& event);

private:
    void _set_default_properties();
    void _setup_connections_ui();
    void _setup_labels();

public:
    BTGraphNode();
    void set_graph_edit(godot::GraphEdit* graph_edit);
    _FORCE_INLINE_ godot::GraphEdit* get_graph_editor() const
    {
        return this->graph_edit;
    }

    virtual void set_task(godot::Ref<BTTask> task);

    _FORCE_INLINE_ godot::Ref<BTTask> get_task() const
    {
        return this->task;
    }

    void focus_task_in_inspector();
protected:
    static void _bind_methods();
};



#endif /* BT_GRAPH_NODE_HPP */