#ifndef BT_GRAPH_NODE_SUBTREE_HPP
#define BT_GRAPH_NODE_SUBTREE_HPP

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/label.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/bt_subtree.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"

class BTGraphNodeSubtree : public BTGraphNode
{
    GDCLASS(BTGraphNodeSubtree, BTGraphNode)

private:
    godot::Label* behaviour_tree_name;
    godot::Label* path_label;

private:
    void _on_gui_input(const godot::Ref<godot::InputEvent>& event) override;
    void _setup_connections_ui();
    void _setup_labels();
public:
    BTGraphNodeSubtree();
    void set_task(godot::Ref<BTTask> task);

protected:
    static void _bind_methods();
};

#endif /* BT_GRAPH_NODE_SUBTREE_HPP */
