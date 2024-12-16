#include "bt_graph_node.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/theme.hpp>

#include "behaviour_tree/tasks/bt_action.hpp"
#include "behaviour_tree/tasks/bt_condition.hpp"
#include "behaviour_tree/utils/utils.hpp"

BTGraphNode::BTGraphNode()
{
    this->_setup_connections_ui();
    this->_setup_icon_label();
    this->_set_default_properties();
    this->ensure_theme();
    this->call_deferred("connect", "gui_input", callable_mp(this, &BTGraphNode::_on_gui_input));
}

void BTGraphNode::ensure_theme()
{
    static godot::Ref<godot::Theme> new_theme;
    if (new_theme.is_null())
    {
        new_theme.instantiate();
        new_theme->add_type("BTGraphNode");
    }
    if (this->get_theme().is_null()) {
        this->set_theme(new_theme);
    }
}

godot::Ref<godot::StyleBoxFlat> create_status_style(const godot::Color& border_color, const godot::Color& bg_color, godot::Side side_to_remove)
{
    godot::Ref<godot::StyleBoxFlat> style;
    style.instantiate();
    ERR_FAIL_COND_V(style.is_null(), nullptr);
    style->set_border_color(border_color);
    style->set_border_width_all(5);
    style->set_border_blend(true);
    style->set_bg_color(bg_color);
    style->set_border_width(side_to_remove, 0);
    return style;
}

godot::Ref<godot::StyleBoxFlat> BTGraphNode::get_style_for_status_panel(const BTTask::Status status)
{
    static godot::Ref<godot::StyleBoxFlat> style_running;
    static godot::Ref<godot::StyleBoxFlat> style_success;
    static godot::Ref<godot::StyleBoxFlat> style_failure;

    if (!style_running.is_valid()) {
        style_running = create_status_style(godot::Color(1.0, 1.0, 0.0), godot::Color::hex(0x2e2e2e), godot::SIDE_TOP);
        style_success = create_status_style(godot::Color(0.0, 1.0, 0.0), godot::Color::hex(0x2e2e2e), godot::SIDE_TOP);
        style_failure = create_status_style(godot::Color(1.0, 0.0, 0.0), godot::Color::hex(0x2e2e2e), godot::SIDE_TOP);
    }

    switch (status)
    {
        case BTTask::Status::RUNNING: return style_running;
        case BTTask::Status::SUCCESS: return style_success;
        case BTTask::Status::FAILURE: return style_failure;
        default: return godot::Ref<godot::StyleBoxFlat>();
    }
}

godot::Ref<godot::StyleBoxFlat> BTGraphNode::get_style_for_status_titlebar(const BTTask::Status status)
{
    static godot::Ref<godot::StyleBoxFlat> style_running;
    static godot::Ref<godot::StyleBoxFlat> style_success;
    static godot::Ref<godot::StyleBoxFlat> style_failure;

    if (!style_running.is_valid()) {
        style_running = create_status_style(godot::Color(1.0, 1.0, 0.0), godot::Color::hex(0x565656), godot::SIDE_BOTTOM);
        style_success = create_status_style(godot::Color(0.0, 1.0, 0.0), godot::Color::hex(0x565656), godot::SIDE_BOTTOM);
        style_failure = create_status_style(godot::Color(1.0, 0.0, 0.0), godot::Color::hex(0x565656), godot::SIDE_BOTTOM);
    }

    switch (status)
    {
        case BTTask::Status::RUNNING: return style_running;
        case BTTask::Status::SUCCESS: return style_success;
        case BTTask::Status::FAILURE: return style_failure;
        default: return godot::Ref<godot::StyleBoxFlat>();
    }
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
    godot::String icon_path = "res://gdextension/behaviour_tree_GDE/icons/";
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
        icon_texture = godot::ResourceLoader::get_singleton()->load(icon_path + "BTDefault.png");
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

void BTGraphNode::set_default_node_color()
{
    this->set_self_modulate(godot::Color::named("WHITE"));
}

void BTGraphNode::set_graph_node_task_status(const BTTask::Status status)
{
    if (status == BTTask::Status::FRESH)
    {
        return;
    }
    godot::Ref<godot::StyleBoxFlat> panel_style = BTGraphNode::get_style_for_status_panel(status);
    godot::Ref<godot::StyleBoxFlat> titlebar_style = BTGraphNode::get_style_for_status_titlebar(status);

    if (panel_style.is_valid() && titlebar_style.is_valid())
    {
        this->add_theme_stylebox_override("panel", panel_style);
        this->add_theme_stylebox_override("titlebar", titlebar_style);
    }
    else
    {
        this->remove_theme_stylebox_override("frame");
        this->remove_theme_stylebox_override("frame");
    }
}

void BTGraphNode::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_graph_node_task_status", "status"), &BTGraphNode::set_graph_node_task_status);

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTGraphNode, STRING_NAME, task_class_name);

    ADD_SIGNAL(MethodInfo("double_clicked"));
    ADD_SIGNAL(MethodInfo("right_clicked"));
}
