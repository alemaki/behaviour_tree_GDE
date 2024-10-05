#include "bt_graph_view.hpp"

bool BTGraphView::has_task_name(const godot::StringName& task_name) const
{
    return this->task_name_to_node.has(task_name);
}

void BTGraphView::create_task_node(const godot::StringName& task_name, const godot::StringName& task_class_name)
{
    ERR_FAIL_COND(task_name.is_empty());
    ERR_FAIL_COND(task_class_name.is_empty());
    ERR_FAIL_COND_MSG(this->has_task_name(task_name), "BTGraphView already has node named: " + task_name + ".");

    BTGraphNode* graph_node = memnew(BTGraphNode);
    this->task_name_to_node[task_name] = graph_node;
    this->add_child(graph_node);
    graph_node->set_task_class_name(task_class_name);
}

void BTGraphView::set_task_title(const godot::StringName& task_name, const godot::String& task_title)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no node named: " + task_name + ".");

    BTGraphNode* graph_node = this->task_name_to_node[task_name];
    graph_node->set_title(task_title);
}

void BTGraphView::delete_task_node(const godot::StringName& task_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no node named: " + task_name + ".");

    BTGraphNode* graph_node = this->task_name_to_node[task_name];
    this->remove_child(graph_node);
    this->task_name_to_node.erase(task_name);
    memdelete(graph_node);
}

void BTGraphView::change_task_class_name(const godot::StringName& task_name, const godot::StringName& task_class_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no node named: " + task_name + ".");

    BTGraphNode* graph_node = this->task_name_to_node[task_name];
    graph_node->set_task_class_name(task_class_name);
}


void BTGraphView::_bind_methods()
{
    
}