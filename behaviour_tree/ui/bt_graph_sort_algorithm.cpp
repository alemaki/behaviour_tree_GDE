#include "bt_graph_sort_algorithm.hpp"

#include <godot_cpp/templates/hash_set.hpp>

bool BTGraphSortAlgorithm::init_tree_utils()
{
    ERR_FAIL_NULL_V(this->root_node, false);
    for (godot::KeyValue<BTGraphNode*, godot::Vector<BTGraphNode*>> key_value : this->parent_to_children)
    {
        ERR_FAIL_NULL_V(key_value.key, false);
        ERR_FAIL_COND_V(key_value.value.size() == 0, false);
        for (BTGraphNode* value : key_value.value)
        {
            ERR_FAIL_NULL_V(value, false);
        }
    }

    this->left_neighbour.clear();
    this->right_neighbour.clear();
    this->parent.clear();

    godot::Vector<godot::Vector<BTGraphNode*>> node_level_array;

    node_level_array.push_back(godot::Vector<BTGraphNode*>({this->root_node}));

    godot::HashSet<BTGraphNode*> occured;
    for (int i = 0; i < node_level_array.size(); i++)
    {
        godot::Vector<BTGraphNode*> current_level_array;
        for (int j = 0; j < node_level_array[i].size(); j++)
        {
            ERR_FAIL_COND_V_MSG(occured.has(node_level_array[i][j]), false, "There is a cycle in the parent_to_children graph.");
            occured.insert(node_level_array[i][j]);
            if (!(this->parent_to_children.has(node_level_array[i][j])))
            {
                continue;
            }
            const godot::Vector<BTGraphNode*>& children = this->parent_to_children[node_level_array[i][j]];
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

    for (const godot::KeyValue<BTGraphNode*, godot::Vector<BTGraphNode*>>& key_value : this->parent_to_children)
    {
        BTGraphNode* parent = key_value.key;
        for (BTGraphNode* child : key_value.value)
        {
            this->parent.insert(child, parent);
        }
    }

    return true;
}

bool BTGraphSortAlgorithm::has_left_sibling(BTGraphNode* node)
{
    ERR_FAIL_NULL_V(node, false);
    return ((this->left_neighbour.has(node)) && (this->parent[this->left_neighbour[node]] == this->parent[node]));
}

bool BTGraphSortAlgorithm::has_right_sibling(BTGraphNode* node)
{
    ERR_FAIL_NULL_V(node, false);
    return ((this->right_neighbour.has(node)) && (this->parent[this->right_neighbour[node]] == this->parent[node]));
}


BTGraphNode* BTGraphSortAlgorithm::get_leftmost(BTGraphNode* node, int level, int depth)
{
    if (level >= depth)
    {
        return node;
    }
    else if (!(parent_to_children.has(node)))
    {
        return nullptr;
    }
    BTGraphNode* rightmost = parent_to_children[node][0];
    BTGraphNode* leftmost = get_leftmost(rightmost, level + 1, depth);
    while (leftmost == nullptr && has_right_sibling(rightmost))
    {
        rightmost = this->right_neighbour[rightmost];
        leftmost = get_leftmost(rightmost, level + 1, depth);
    }
    return leftmost;
}

bool BTGraphSortAlgorithm::apportion(BTGraphNode* node, int level)
{
    ERR_FAIL_COND_V(node == nullptr, false);
    ERR_FAIL_COND_V(!(this->parent_to_children.has(node)), false);
    ERR_FAIL_COND_V(this->left_neighbour[node] == nullptr, false);

    int portion = 0;
    BTGraphNode* leftmost = this->parent_to_children[node][0];
    BTGraphNode* neighbour = this->left_neighbour[leftmost];
    int compare_depth = 1;
    int depth_to_stop = 200 - level;
    while (leftmost != nullptr && neighbour != nullptr && compare_depth <= depth_to_stop)
    {
        int left_modsum = 0;
        int right_modsum = 0;
        BTGraphNode* ancestor_leftmost = leftmost;
        BTGraphNode* ancestor_neighbour = neighbour;
        for (int i = 0; i < compare_depth; i++)
        {
            ERR_FAIL_COND_V(!(this->parent.has(ancestor_leftmost)), false);
            ERR_FAIL_COND_V(!(this->parent.has(ancestor_neighbour)), false);
            ancestor_leftmost = this->parent[ancestor_leftmost];
            ancestor_neighbour = this->parent[ancestor_neighbour];

            right_modsum += this->modifier[ancestor_leftmost];
            left_modsum += this->modifier[ancestor_neighbour];
        }
        int move_distance = (this->prelim[neighbour] +
                             left_modsum +
                             this->subtree_separation +
                             (leftmost->get_size().y + neighbour->get_size().y)/2 -
                             (this->prelim[leftmost] + right_modsum));  
        if (move_distance > 0)
        {
            BTGraphNode* temp_node = node;
            int left_siblings = 0;
            while (temp_node != nullptr && temp_node != ancestor_neighbour)
            {
                left_siblings++;
                if (has_left_sibling(temp_node))
                {
                    temp_node = this->left_neighbour[temp_node];
                }
                else
                {
                    temp_node = nullptr;
                }
            }

            if (temp_node != nullptr)
            {
                portion = move_distance/left_siblings;
                temp_node = node;
                while (temp_node != ancestor_neighbour)
                {
                    this->prelim[temp_node] += move_distance;
                    this->modifier[temp_node] += move_distance;
                    move_distance -= portion;
                    temp_node = this->left_neighbour[temp_node];
                }
            }
            else
            {
                return true;
            }
        }
        compare_depth++;
        if (leftmost->get_task()->get_child_count() == 0)
        {
            leftmost = this->get_leftmost(node, 0, compare_depth);
        }
        else
        {
            ERR_FAIL_COND_V(!(this->parent_to_children.has(leftmost)), false);
            leftmost = this->parent_to_children[leftmost][0];
        }
        neighbour = this->left_neighbour[leftmost];
    }
    return true;
}

bool BTGraphSortAlgorithm::first_walk(BTGraphNode* node, int level)
{
    ERR_FAIL_COND_V(node == nullptr, false);
    if (!(this->parent_to_children.has(node)))
    {
        if (has_left_sibling(node))
        {
            this->prelim[node] = this->prelim[this->left_neighbour[node]] +
                                 this->sibling_separation +
                                 (node->get_size().y + this->left_neighbour[node]->get_size().y)/2;
        }
        else
        {
            this->prelim[node] = 0;
        }
    }
    else
    {
        int size = this->parent_to_children[node].size();
        for (BTGraphNode* child : this->parent_to_children[node])
        {
            first_walk(child, level + 1);
        }
        int midpoint = (this->prelim[this->parent_to_children[node][0]] + this->prelim[this->parent_to_children[node][size - 1]])/2;
        if (has_left_sibling(node))
        {
            this->prelim[node] = this->prelim[this->left_neighbour[node]] +
                                 this->sibling_separation +
                                 (node->get_size().y + this->left_neighbour[node]->get_size().y)/2;
            this->modifier[node] = this->prelim[node] - midpoint;
            apportion(node, level);
        }
        else
        {
            this->prelim[node] = midpoint;
        }
    }
    return true;
}

godot::HashMap<BTGraphNode*, godot::Vector2> BTGraphSortAlgorithm::get_arranged_nodes_positions()
{
    ERR_FAIL_NULL_V(this->root_node, {});
    this->result = {};

    this->init_tree_utils();

    return {};
}


void BTGraphSortAlgorithm::_bind_methods()
{
    
}