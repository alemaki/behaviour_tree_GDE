#ifndef BT_GRAPH_NODE_HPP
#define BT_GRAPH_NODE_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/graph_node.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include "behaviour_tree/behaviour_tree.hpp"


class BTGraphNode : public godot::GraphNode
{
    GDCLASS(BTGraphNode, godot::GraphNode)

private:
    godot::GraphEdit* graph_editor;
    godot::LineEdit* rename_edit;
    godot::Ref<BTTask> task;
    
private:
    void setup_default();
    void _on_rename_edit_text_submitted(const godot::String& new_text);
    void _on_gui_input(const godot::Ref<godot::InputEvent>& event);

public:
    BTGraphNode();
    void set_graph_editor(godot::GraphEdit* graph_editor);
    _FORCE_INLINE_ godot::GraphEdit* get_graph_editor() const
    {
        return this->graph_editor;
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