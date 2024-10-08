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

void BTGraphView::connect_task_nodes(const godot::StringName& parent_task_name, const godot::StringName& child_task_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(parent_task_name)), "BTGraphView has no parent node named: " + parent_task_name + ".");
    ERR_FAIL_COND_MSG(!(this->has_task_name(child_task_name)), "BTGraphView has no child node named: " + child_task_name + ".");
    
    godot::StringName parent_node_name = this->task_name_to_node[parent_task_name]->get_name();
    godot::StringName child_node_name = this->task_name_to_node[child_task_name]->get_name();

    // TODO: fix port numbers?
    this->connect_node(parent_node_name, 0, child_node_name, 0);
}

void BTGraphView::disconnect_task_nodes(const godot::StringName& parent_task_name, const godot::StringName& child_task_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(parent_task_name)), "BTGraphView has no parent node named: " + parent_task_name + ".");
    ERR_FAIL_COND_MSG(!(this->has_task_name(child_task_name)), "BTGraphView has no child node named: " + child_task_name + ".");
    
    godot::StringName parent_node_name = this->task_name_to_node[parent_task_name]->get_name();
    godot::StringName child_node_name = this->task_name_to_node[child_task_name]->get_name();

    // TODO: fix port numbers?
    this->disconnect_node(parent_node_name, 0, child_node_name, 0);
}

void BTGraphView::set_node_position(const godot::StringName& task_name, godot::Vector2 position)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no node named: " + task_name + ".");

    BTGraphNode* node = this->task_name_to_node[task_name];
    node->set_position_offset(position);
}

void BTGraphView::_bind_methods()
{
    
}