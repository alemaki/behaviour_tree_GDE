#include "bt_graph_node.hpp"

BTGraphNode::BTGraphNode()
{

}

BTGraphNode::BTGraphNode(const godot::StringName& name, godot::Ref<BTTask> task, godot::GraphEdit* graph_editor)
{
    this->set_name(name);
    this->set_title(godot::String("[") + name + godot::String("]"));
    this->set_task(task);
    this->set_graph_editor(graph_editor);
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