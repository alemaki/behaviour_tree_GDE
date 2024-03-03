#include "bt_graph_node.hpp"

BTGraphNode::BTGraphNode()
{
    this->setup_default();
}

void BTGraphNode::setup_default()
{
    godot::Control* control = memnew(godot::Control);
    /* TODO: make it look better */
    this->add_child(control);

    this->set_slot(0, true, 0, godot::Color::named("WHITE"),
                      true, 1, godot::Color::named("WHITE"));

    this->set_resizable(false);
    this->set_custom_minimum_size(godot::Size2(100, 30));

    this->set_position_offset(godot::Vector2(100, 100));

    this->rename_edit = memnew(godot::LineEdit);

    this->rename_edit->connect("text_submitted", callable_mp(this, &BTGraphNode::_on_rename_edit_text_submitted));
    this->connect("gui_input", callable_mp(this, &BTGraphNode::_on_gui_input));
}

void BTGraphNode::set_graph_editor(godot::GraphEdit* graph_editor)
{
    this->graph_editor = graph_editor;
}

void BTGraphNode::set_task(godot::Ref<BTTask> task)
{
    this->task = task;
}

void BTGraphNode::_on_rename_edit_text_submitted(const godot::String& new_text)
{
    this->set_title(new_text);
    this->task->set_custom_name(new_text);
    this->rename_edit->set_visible(false);
}

void BTGraphNode::_on_gui_input(const godot::Ref<godot::InputEvent>& event)
{
    godot::Ref<godot::InputEventMouseButton> click_event = godot::Object::cast_to<godot::InputEventMouseButton>(event.ptr());
    if (click_event != nullptr && click_event->is_double_click())
    {
        this->rename_edit->set_text(this->get_name());
        this->rename_edit->set_visible(true);
        /* grab_focus should be a deferred call. See:
         * https://docs.godotengine.org/en/stable/classes/class_control.html#class-control-method-grab-focus */
        this->rename_edit->call_deferred("grab_focus");
        this->rename_edit->set_position(godot::Vector2(this->get_position().x, this->get_position().y));
    }
}

void BTGraphNode::_bind_methods()
{
    using namespace godot;

    
    ClassDB::bind_method(D_METHOD("set_graph_editor", "graph_editor"), &BTGraphNode::set_graph_editor);
    ClassDB::bind_method(D_METHOD("get_graph_editor"), &BTGraphNode::get_graph_editor);
    ClassDB::bind_method(D_METHOD("set_task", "task"), &BTGraphNode::set_task);
    ClassDB::bind_method(D_METHOD("get_task"), &BTGraphNode::get_task);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "graph_editor"), "set_graph_editor", "get_graph_editor");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "task"), "set_task", "get_task");
}
