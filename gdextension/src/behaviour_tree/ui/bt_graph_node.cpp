#include "bt_graph_node.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/composites/bt_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_random_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_random_sequence.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_fail.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_succeed.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"
#include "behaviour_tree/tasks/decorators/bt_probability.hpp"
#include "behaviour_tree/tasks/decorators/bt_repeat.hpp"
#include "behaviour_tree/tasks/bt_action.hpp"

BTGraphNode::BTGraphNode()
{
    this->setup_default();
}

void BTGraphNode::_setup_connections_ui()
{
    godot::Control* control = memnew(godot::Control);
    /* TODO: make it look better */
    this->add_child(control);

    this->set_slot(0, true, 0, godot::Color::named("WHITE"),
                      true, 1, godot::Color::named("WHITE"));

}

void BTGraphNode::_setup_task_type_option_button()
{
    this->task_type_opition_button = memnew(godot::OptionButton);
    this->add_child(this->task_type_opition_button);

    this->task_type_opition_button->add_item(BTSelector::get_class_static());
    this->task_type_opition_button->add_item(BTSequence::get_class_static());
    this->task_type_opition_button->add_item(BTRandomSelector::get_class_static());
    this->task_type_opition_button->add_item(BTRandomSequence::get_class_static());

    this->task_type_opition_button->add_item(BTAlwaysFail::get_class_static());
    this->task_type_opition_button->add_item(BTAlwaysSucceed::get_class_static());
    this->task_type_opition_button->add_item(BTInvert::get_class_static());
    this->task_type_opition_button->add_item(BTProbability::get_class_static());
    this->task_type_opition_button->add_item(BTRepeat::get_class_static());

    this->task_type_opition_button->add_item(BTAction::get_class_static());


    this->task_type_opition_button->call_deferred("connect", "item_selected", this, "_task_type_item_selected");
}

void BTGraphNode::_task_type_item_selected(int id)
{
    int index = this->task_type_opition_button->get_item_index(id);
    godot::String class_name = this->task_type_opition_button->get_item_text(index);

    ERR_FAIL_COND_MSG(this->task.is_null(), "No task.");
    if (this->task->get_class_static() != class_name)
    {
        godot::Ref<BTTask> new_task = godot::ClassDB::instantiate(class_name);
        new_task->set_custom_name(this->task->get_custom_name());
        new_task->set_children(this->task->get_children());
        new_task->set_parent(this->task->get_parent());
        
    }

}

void BTGraphNode::setup_default()
{
    this->_setup_connections_ui();
    this->_setup_task_type_option_button();

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
