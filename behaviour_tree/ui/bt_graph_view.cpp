#include "bt_graph_view.hpp"
#include <godot_cpp/templates/hash_set.hpp>

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


bool BTGraphView::init_tree_utils(
    TreeArrangeUtils& utils,
    const godot::StringName& root_task_name,
    const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const
{
    ERR_FAIL_COND_V_MSG(!(this->has_task_name(root_task_name)), false, "BTGraphView has no node named: " + root_task_name + ".");

    for (const godot::KeyValue<StringName, godot::Vector<StringName>>& key_value : parent_to_children_names)
    {
        ERR_FAIL_COND_V_MSG(!(this->has_task_name(key_value.key)), false, "BTGraphView has no node named: " + key_value.key + ".");
        
        for (const godot::StringName& child_name: key_value.value)
        {
            ERR_FAIL_COND_V_MSG(!(this->has_task_name(child_name)), false, "BTGraphView has no node named: " + child_name + ".");
        }
    }

    utils.left_neighbour.clear();
    utils.right_neighbour.clear();
    utils.parent.clear();

    godot::Vector<godot::Vector<StringName>> node_level_array;

    node_level_array.push_back(godot::Vector<StringName>({root_task_name}));

    godot::HashSet<StringName> occured;
    for (int i = 0; i < node_level_array.size(); i++)
    {
        godot::Vector<StringName> current_level_array;
        for (int j = 0; j < node_level_array[i].size(); j++)
        {
            ERR_FAIL_COND_V_MSG(occured.has(node_level_array[i][j]), false, "There is a cycle in the parent_to_children_names graph: " + node_level_array[i][j] + " is in the cycle.");
            occured.insert(node_level_array[i][j]);
            if (!(parent_to_children_names.has(node_level_array[i][j])))
            {
                continue;
            }
            const godot::Vector<StringName>& children = parent_to_children_names[node_level_array[i][j]];
            current_level_array.append_array(children);
        }
        if (!current_level_array.is_empty())
        {
            node_level_array.push_back(current_level_array);
        }
    }

    for (int i = 0; i < node_level_array.size(); i++)
    {
        for (int j = 0; j < node_level_array[i].size() - 1; j++)
        {
            BTGraphNode* left_node = this->task_name_to_node[node_level_array[i][j]];
            BTGraphNode* right_node = this->task_name_to_node[node_level_array[i][j + 1]];
            utils.left_neighbour[right_node] = left_node;
            utils.right_neighbour[left_node] = right_node;
            utils.prelim[left_node] = 0;
            utils.prelim[left_node] = 0;
        }
        BTGraphNode* node = this->task_name_to_node[node_level_array[i][node_level_array[i].size() - 1]];
        utils.prelim[node] = 0;
        utils.prelim[node] = 0;
    }

    for (const godot::KeyValue<StringName, godot::Vector<StringName>>& key_value : parent_to_children_names)
    {
        BTGraphNode* parent = this->task_name_to_node[key_value.key];
        for (const godot::StringName& child_name: key_value.value)
        {
            BTGraphNode* child = this->task_name_to_node[child_name];
            utils.parent[child] = parent;
        }
    }

    return true;
}

godot::HashMap<BTGraphNode*, godot::Vector2> BTGraphView::get_arranged_nodes_positions(
    const godot::StringName& root_task_name,
    const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const
{
    godot::HashMap<BTGraphNode*, godot::Vector2> result;
    BTGraphNode* root_node = this->task_name_to_node[root_task_name];

    BTGraphView::TreeArrangeUtils utils;

    return {};
}

void BTGraphView::_bind_methods()
{
    
}