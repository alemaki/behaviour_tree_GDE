#ifndef BT_GRAPH_SORT_ALGORITHM_HPP
#define BT_GRAPH_SORT_ALGORITHM_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include "behaviour_tree/ui/bt_graph_node.hpp"

class BTGraphSortAlgorithm : public godot::Resource
{
    GDCLASS(BTGraphSortAlgorithm, godot::Resource)

private:
    godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;

public:
    godot::HashMap<BTGraphNode*, BTGraphNode*> left_neighbour;
    godot::HashMap<BTGraphNode*, BTGraphNode*> right_neighbour;
    godot::HashMap<BTGraphNode*, BTGraphNode*> parent;
    godot::HashMap<BTGraphNode*, int> prelim;
    godot::HashMap<BTGraphNode*, int> modifier;
    int sibling_separation = 20;
    int subtree_separation = 50;
    int level_separation = 220;
    //TODO: fix magic numbers.

    bool init_tree_utils(BTGraphNode* root_node, const godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>>& parent_to_children);
    godot::HashMap<BTGraphNode*, godot::Vector2> get_arranged_nodes_positions(BTGraphNode* root_node, const godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>>& parent_to_children);

protected:
    static void _bind_methods();
};

#endif /* BT_GRAPH_SORT_ALGORITHM_HPP */