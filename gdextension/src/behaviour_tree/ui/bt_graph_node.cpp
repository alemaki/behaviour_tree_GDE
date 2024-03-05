#include "bt_graph_node.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
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

    this->call_deferred("connect", "gui_input", callable_mp(this, &BTGraphNode::_on_gui_input));
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
    this->task = task;
}

void BTGraphNode::_on_gui_input(const godot::Ref<godot::InputEvent>& event)
{
    godot::Ref<godot::InputEventMouseButton> click_event = godot::Object::cast_to<godot::InputEventMouseButton>(event.ptr());
    if (click_event != nullptr && click_event->is_double_click())
    {
        this->emit_signal("double_clicked", this);
    }
}

void BTGraphNode::_bind_methods()
{
    using namespace godot;

    
    ClassDB::bind_method(D_METHOD("set_graph_edit", "graph_edit"), &BTGraphNode::set_graph_edit);
    ClassDB::bind_method(D_METHOD("get_graph_editor"), &BTGraphNode::get_graph_editor);
    ClassDB::bind_method(D_METHOD("set_task", "task"), &BTGraphNode::set_task);
    ClassDB::bind_method(D_METHOD("get_task"), &BTGraphNode::get_task);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "task"), "set_task", "get_task");

    ADD_SIGNAL(MethodInfo("double_clicked", PropertyInfo(Variant::OBJECT, "BTGraphNode", PROPERTY_HINT_NONE, "Control")));
}
