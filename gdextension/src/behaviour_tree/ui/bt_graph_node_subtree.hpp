#ifndef BT_GRAPH_NODE_SUBTREE_HPP
#define BT_GRAPH_NODE_SUBTREE_HPP

#include <godot_cpp/classes/label.hpp>


#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/bt_subtree.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/behaviour_tree.hpp"

class BTGraphNodeSubtree : public BTGraphNode
{
    GDCLASS(BTGraphNodeSubtree, BTGraphNode)

private:
    godot::Label* path_label;

private:
    void _setup_subtree_labels();
public:
    BTGraphNodeSubtree();
    void set_task(godot::Ref<BTTask> task) override;
    void set_file_path(const godot::String& path);
    _FORCE_INLINE_ godot::String get_file_path()
    {
        godot::Ref<BTSubtree> subtree_task = godot::Ref<BTSubtree>(this->task);
        return subtree_task == nullptr ? "" : subtree_task->get_file_path();
    }

protected:
    static void _bind_methods();
};

#endif /* BT_GRAPH_NODE_SUBTREE_HPP */
