#ifndef BT_GRAPH_SORT_ALGORITHM_HPP
#define BT_GRAPH_SORT_ALGORITHM_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include "behaviour_tree/ui/bt_graph_node.hpp"

/*  
    Node sorting algorithm based on John Q. Walker II's general tree node positioning algorithm.  
    More information can be found here:  
    https://jacobfilipp.com/DrDobbs/articles/CUJ/1991/9102/walker/walker.htm (last visited: 13.03.2025)  

    Usage:  
    - Call `get_arranged_nodes_position()` for automatic execution.  
    - Alternatively, manually invoke the following functions in order:  
      1. `init_tree_utils()`  
      2. `first_walk()`  
      3. `second_walk()`  
*/

class BTGraphSortAlgorithm : public godot::Resource
{
    GDCLASS(BTGraphSortAlgorithm, godot::Resource)

    using ParentToChildrenMap = godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>>;

private:
    BTGraphNode* root_node = nullptr;
    ParentToChildrenMap parent_to_children = {};

    godot::HashMap<BTGraphNode*, godot::Vector2> result = {};

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

    CREATE_GETTER_SETTER_DEFAULT(BTGraphNode*, root_node);
    CREATE_GETTER_SETTER_DEFAULT(ParentToChildrenMap, parent_to_children);

    bool init_tree_utils();
    bool has_left_sibling(BTGraphNode* node);
    bool has_right_sibling(BTGraphNode* node);
    BTGraphNode* get_leftmost(BTGraphNode* node, int level, int depth);
    bool apportion(BTGraphNode* node, int level);
    bool first_walk(BTGraphNode* node, int level = 0);
    bool second_walk(BTGraphNode* node, godot::HashMap<BTGraphNode*, godot::Vector2>& result, int level = 0, int modsum = 0);
    
    godot::HashMap<BTGraphNode*, godot::Vector2> get_arranged_nodes_position();

protected:
    static void _bind_methods();
};

#endif /* BT_GRAPH_SORT_ALGORITHM_HPP */