#ifndef BT_GRAPH_NODE_HPP
#define BT_GRAPH_NODE_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/graph_node.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/option_button.hpp>
#include <godot_cpp/templates/hash_map.hpp>

class BTGraphNode : public godot::GraphNode
{
    GDCLASS(BTGraphNode, godot::GraphNode)

private:
    godot::GraphEdit* graph_edit;
    godot::Ref<BTTask> task;
    godot::OptionButton* task_type_opition_button;

private:
    void setup_default();
    void _on_gui_input(const godot::Ref<godot::InputEvent>& event);
    void _setup_connections_ui();
    void _setup_task_type_option_button();
    void _task_type_item_selected(int index);
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

#endif //BT_GRAPH_NODE_HPP