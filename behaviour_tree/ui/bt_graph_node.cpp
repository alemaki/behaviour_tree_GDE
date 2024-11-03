#include "bt_graph_node.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include "behaviour_tree/tasks/bt_action.hpp"
#include "behaviour_tree/tasks/bt_condition.hpp"
#include "behaviour_tree/utils/utils.hpp"

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
    godot::String icon_path = "res://gdextension/behaviour_tree/icons/";
    godot::String extension = ".png";
    godot::Ref<godot::Texture2D> icon_texture;
    
    if (godot::ResourceLoader::get_singleton()->exists(icon_path + this->task_class_name + extension))
    {
        icon_texture = godot::ResourceLoader::get_singleton()->load(icon_path + this->task_class_name + extension);
    }
    else if ((godot::ResourceLoader::get_singleton()->exists(icon_path + BTAction::get_class_static() + extension))
            && (utils::is_subclass(this->task_class_name, BTAction::get_class_static())))
    {
        icon_texture = godot::ResourceLoader::get_singleton()->load(icon_path + BTAction::get_class_static() + extension);
    }
    else if ((godot::ResourceLoader::get_singleton()->exists(icon_path + BTAction::get_class_static() + extension))
            && (utils::is_subclass(this->task_class_name, BTCondition::get_class_static())))
    {
        icon_texture = godot::ResourceLoader::get_singleton()->load(icon_path + BTCondition::get_class_static() + extension);
    }
    else
    {
        icon_texture = godot::ResourceLoader::get_singleton()->load("res://gdextension/behaviour_tree/icons/BTDefault.png");
    }
    
    this->icon->set_texture(icon_texture);
}

void BTGraphNode::set_task_class_name(const godot::StringName& task_class_name)
{
    this->task_class_name = task_class_name;
    this->task_type_label->set_text(task_class_name);
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

void BTGraphNode::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTGraphNode, STRING_NAME, task_class_name);

    ADD_SIGNAL(MethodInfo("double_clicked"));
    ADD_SIGNAL(MethodInfo("right_clicked"));
}
