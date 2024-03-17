#include "bt_graph_node.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

BTGraphNode::BTGraphNode()
{
    this->_setup_connections_ui();
    this->_setup_task_type_label();

    this->set_resizable(false);
    this->set_custom_minimum_size(godot::Size2(100, 30));

    this->set_position_offset(godot::Vector2(100, 100));

    this->call_deferred("connect", "gui_input", callable_mp(this, &BTGraphNode::_on_gui_input));
}

void BTGraphNode::_setup_connections_ui()
{
    godot::Control* control = memnew(godot::Control);
    /* TODO: make it look better */
    this->add_child(control);

    this->set_slot(0, true, 0, godot::Color::named("WHITE"),
                      true, 1, godot::Color::named("WHITE"));

}

void BTGraphNode::_setup_task_type_label()
{
    this->task_type_label = memnew(godot::Label);
    this->add_child(this->task_type_label);
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
    godot::StringName class_name = task->get_class();

    godot::Vector<godot::StringName> task_names = 
    {
        BTTask::get_class_static(),
        BTSelector::get_class_static(),
        BTSequence::get_class_static(),
        BTRandomSelector::get_class_static(),
        BTRandomSequence::get_class_static(),
        BTAlwaysFail::get_class_static(),
        BTAlwaysSucceed::get_class_static(),
        BTInvert::get_class_static(),
        BTProbability::get_class_static(),
        BTRepeat::get_class_static(),
        BTAction::get_class_static(),
    };

    int id = task_names.find(class_name);
    ERR_FAIL_COND_MSG(id == -1, "Task name doesn't exist.");

    this->task_type_label->set_text(class_name);
    this->task = task;
}

void BTGraphNode::_on_gui_input(const godot::Ref<godot::InputEvent>& event)
{
    godot::Ref<godot::InputEventMouseButton> click_event = godot::Object::cast_to<godot::InputEventMouseButton>(event.ptr());
    if (click_event != nullptr && click_event->is_double_click())
    {
        this->emit_signal("double_clicked", this);
    }
    if (click_event != nullptr && click_event->get_button_index() == godot::MOUSE_BUTTON_RIGHT && click_event->is_released())
    {
        this->emit_signal("right_clicked", this);
    }
}

void BTGraphNode::_bind_methods()
{
    using namespace godot;

    
    ClassDB::bind_method(D_METHOD("set_graph_edit", "graph_edit"), &BTGraphNode::set_graph_edit);
    ClassDB::bind_method(D_METHOD("get_graph_editor"), &BTGraphNode::get_graph_editor);
    ClassDB::bind_method(D_METHOD("set_task", "task"), &BTGraphNode::set_task);
    ClassDB::bind_method(D_METHOD("get_task"), &BTGraphNode::get_task);

    ADD_SIGNAL(MethodInfo("double_clicked", PropertyInfo(Variant::OBJECT, "BTGraphNode", PROPERTY_HINT_NONE, "Control")));
    ADD_SIGNAL(MethodInfo("right_clicked", PropertyInfo(Variant::OBJECT, "BTGraphNode", PROPERTY_HINT_NONE, "Control")));
}
