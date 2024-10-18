#include "bt_graph_view.hpp"
#include "behaviour_tree/ui/bt_graph_sort_algorithm.hpp"

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

godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> BTGraphView::get_node_tree_map(const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const
{
    godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;

    for (const godot::KeyValue<StringName, godot::Vector<StringName>>& key_value : parent_to_children_names)
    {
        ERR_FAIL_COND_V_MSG(!(this->has_task_name(key_value.key)), {}, "BTGraphView has no node named: " + key_value.key + ".");
        BTGraphNode* parent = this->task_name_to_node[key_value.key];
        parent_to_children.insert(parent, {});
        
        for (const godot::StringName& child_name: key_value.value)
        {
            ERR_FAIL_COND_V_MSG(!(this->has_task_name(child_name)), {}, "BTGraphView has no node named: " + child_name + ".");
            parent_to_children[parent].push_back(this->task_name_to_node[child_name]);
        }
    }
    return parent_to_children;
}

godot::HashMap<BTGraphNode*, godot::Vector2> BTGraphView::get_arranged_nodes_positions(
    const godot::StringName& root_task_name,
    const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const
{
    ERR_FAIL_COND_V_MSG(!(this->has_task_name(root_task_name)), false, "BTGraphView has no node named: " + root_task_name + ".");

    BTGraphNode* root = this->task_name_to_node[root_task_name];
    godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children = this->get_node_tree_map(parent_to_children_names);

    godot::Ref<BTGraphSortAlgorithm> algorithm = memnew(BTGraphSortAlgorithm);

    algorithm->set_root_node(root);
    algorithm->set_parent_to_children(parent_to_children);

    return algorithm->get_arranged_nodes_positions();
}


void BTGraphView::arrange_nodes(
    const godot::StringName& root_task_name,
    const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names)
{
    godot::HashMap<BTGraphNode*, godot::Vector2> positons = this->get_arranged_nodes_positions(root_task_name, parent_to_children_names);

    for (const godot::KeyValue<BTGraphNode*, godot::Vector2>& key_value: positons)
    {
        key_value.key->set_position_offset(key_value.value);
    }
}

void BTGraphView::_bind_methods()
{
    
}