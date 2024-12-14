#ifndef BT_GRAPH_NODE_HPP
#define BT_GRAPH_NODE_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/graph_node.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/color_rect.hpp>

#include "behaviour_tree/utils/macros.hpp"

class BTGraphNode : public godot::GraphNode
{
    GDCLASS(BTGraphNode, godot::GraphNode)

protected:
    godot::HBoxContainer* icon_name_container = nullptr;
    godot::Label* task_type_label = nullptr;
    godot::TextureRect* icon = nullptr;
    godot::StringName task_class_name;
    godot::ColorRect* border_rect = nullptr;

private:
    void _on_gui_input(const godot::Ref<godot::InputEvent>& event);

private:
    void _set_default_properties();
    void _setup_connections_ui();
    void _setup_icon_label();

    void evaluate_icon();

public:
    BTGraphNode();
    virtual void set_task_class_name(const godot::StringName& task_class_name);
    _FORCE_INLINE_ godot::StringName get_task_class_name()
    {
        return this->task_class_name;
    }

    virtual void set_default_node_color();

    void set_border_color(const godot::Color& color);

protected:
    static void _bind_methods();
};



#endif /* BT_GRAPH_NODE_HPP */