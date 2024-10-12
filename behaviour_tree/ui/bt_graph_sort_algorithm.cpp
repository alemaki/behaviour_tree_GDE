#include "bt_graph_sort_algorithm.hpp"

#include <godot_cpp/templates/hash_set.hpp>

bool BTGraphSortAlgorithm::init_tree_utils(
    BTGraphNode* root_node,
    const godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>>& parent_to_children)
{
    ERR_FAIL_NULL_V(root_node, false);
    for (godot::KeyValue<BTGraphNode*, godot::Vector<BTGraphNode*>> key_value : parent_to_children)
    {
        ERR_FAIL_NULL_V(key_value.key, false);
        for (BTGraphNode* value : key_value.value)
        {
            ERR_FAIL_NULL_V(value, false);
        }
    }

    this->left_neighbour.clear();
    this->right_neighbour.clear();
    this->parent.clear();

    godot::Vector<godot::Vector<BTGraphNode*>> node_level_array;

    node_level_array.push_back(godot::Vector<BTGraphNode*>({root_node}));

    godot::HashSet<BTGraphNode*> occured;
    for (int i = 0; i < node_level_array.size(); i++)
    {
        godot::Vector<BTGraphNode*> current_level_array;
        for (int j = 0; j < node_level_array[i].size(); j++)
        {
            ERR_FAIL_COND_V_MSG(occured.has(node_level_array[i][j]), false, "There is a cycle in the parent_to_children graph.");
            occured.insert(node_level_array[i][j]);
            if (!(parent_to_children.has(node_level_array[i][j])))
            {
                continue;
            }
            const godot::Vector<BTGraphNode*>& children = parent_to_children[node_level_array[i][j]];
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
            BTGraphNode* left_node =node_level_array[i][j];
            BTGraphNode* right_node = node_level_array[i][j + 1];
            this->left_neighbour.insert(right_node, left_node);
            this->right_neighbour.insert(left_node, right_node);
            this->prelim.insert(left_node, 0);
            this->modifier.insert(right_node, 0);
        }
        BTGraphNode* node = node_level_array[i][node_level_array[i].size() - 1];
        this->prelim.insert(node, 0);
        this->modifier.insert(node, 0);
    }

    for (const godot::KeyValue<BTGraphNode*, godot::Vector<BTGraphNode*>>& key_value : parent_to_children)
    {
        BTGraphNode* parent = key_value.key;
        for (BTGraphNode* child : key_value.value)
        {
            this->parent.insert(child, parent);
        }
    }

    return true;
}

godot::HashMap<BTGraphNode*, godot::Vector2> BTGraphSortAlgorithm::get_arranged_nodes_positions(
    BTGraphNode* root_node, 
    const godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>>& parent_to_children)
{
    godot::HashMap<BTGraphNode*, godot::Vector2> result;

    this->init_tree_utils(root_node, parent_to_children);

    return {};
}


void BTGraphSortAlgorithm::_bind_methods()
{
    
}