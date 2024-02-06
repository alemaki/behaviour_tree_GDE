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
    this->set_custom_minimum_size(godot::Size2(100, 10));

    this->set_position_offset(godot::Vector2(100, 100));
}

void BTGraphNode::set_graph_editor(godot::GraphEdit* graph_editor)
{
    this->graph_editor = graph_editor;
}

void BTGraphNode::set_task(godot::Ref<BTTask> task)
{
    this->task = task;
    this->task->emit_changed();
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