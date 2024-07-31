#include "bt_graph_node.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

BTGraphNode::BTGraphNode()
{
    this->_setup_connections_ui();
    this->_setup_icon_label();
    this->_set_default_properties();

    this->call_deferred("connect", "gui_input", callable_mp(this, &BTGraphNode::_on_gui_input));
}

void BTGraphNode::_set_default_properties()
{
    this->set_resizable(false);
    this->set_custom_minimum_size(godot::Size2(100, 30));

    this->set_position_offset(godot::Vector2(100, 100));
}

void BTGraphNode::_setup_connections_ui()
{
    godot::Control* control = memnew(godot::Control);
    /* TODO: make it look better */
    this->add_child(control);

    this->set_slot(0, true, 0, godot::Color::named("WHITE"),
                      true, 1, godot::Color::named("WHITE"));

}

void BTGraphNode::_setup_icon_label()
{
    this->icon_name_container = memnew(godot::HBoxContainer);
    this->icon = memnew(godot::TextureRect);
    this->icon->set_expand_mode(godot::TextureRect::ExpandMode::EXPAND_FIT_WIDTH);
    this->icon_name_container->add_child(this->icon);
    this->task_type_label = memnew(godot::Label);
    this->icon_name_container->add_child(this->task_type_label);
    this->add_child(icon_name_container);
    this->evaluate_icon();
}

void BTGraphNode::evaluate_icon()
{
    godot::Ref<godot::Texture2D> icon_texture;
    if (this->task != nullptr)
    {
        godot::String class_name = this->task->get_class();
        if (godot::ResourceLoader::get_singleton()->exists("res://gdextension/behaviour_tree/icons/" + class_name + ".png"))
        {
            icon_texture = godot::ResourceLoader::get_singleton()->load("res://gdextension/behaviour_tree/icons/" + class_name + ".png");
        }
        else
        {
            icon_texture = godot::ResourceLoader::get_singleton()->load("res://gdextension/behaviour_tree/icons/BTDefault.png");
        }
    }
    this->icon->set_texture(icon_texture);
}

void BTGraphNode::set_graph_edit(godot::GraphEdit* graph_edit)
{
    if (this->graph_edit != nullptr)
    {
        return;
    } 
    this->graph_edit = graph_edit;
}

void BTGraphNode::set_task(godot::Ref<BTTask> task)
{
    ERR_FAIL_COND(task.is_null());

    godot::StringName class_name = task->get_class();

    this->task_type_label->set_text(class_name);
    this->task = task;
    this->evaluate_icon(); 
}

void BTGraphNode::_on_gui_input(const godot::Ref<godot::InputEvent>& event)
{
    godot::Ref<godot::InputEventMouseButton> click_event = godot::Object::cast_to<godot::InputEventMouseButton>(event.ptr());
    if (click_event != nullptr && click_event->is_double_click())
    {
        this->emit_signal("double_clicked");
    }
    if (click_event != nullptr && click_event->get_button_index() == godot::MOUSE_BUTTON_RIGHT && click_event->is_released())
    {
        this->emit_signal("right_clicked");
    }
}

void BTGraphNode::focus_task_in_inspector()
{
    ERR_FAIL_COND(this->task.is_null());

    godot::EditorInterface* editor_interface = godot::EditorInterface::get_singleton();
    editor_interface->inspect_object(task.ptr());
}

void BTGraphNode::_bind_methods()
{
    using namespace godot;

    
    ClassDB::bind_method(D_METHOD("set_graph_edit", "graph_edit"), &BTGraphNode::set_graph_edit);
    ClassDB::bind_method(D_METHOD("get_graph_editor"), &BTGraphNode::get_graph_editor);
    ClassDB::bind_method(D_METHOD("set_task", "task"), &BTGraphNode::set_task);
    ClassDB::bind_method(D_METHOD("get_task"), &BTGraphNode::get_task);

    ADD_SIGNAL(MethodInfo("double_clicked"));
    ADD_SIGNAL(MethodInfo("right_clicked"));
}
