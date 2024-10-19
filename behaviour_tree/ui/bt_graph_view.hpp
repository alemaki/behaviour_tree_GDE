#ifndef BT_GRAPH_VIEW_HPP
#define BT_GRAPH_VIEW_HPP

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/editor_undo_redo_manager.hpp>
#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/popup_menu.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include "behaviour_tree/ui/bt_graph_sort_algorithm.hpp"

#include "behaviour_tree/behaviour_tree.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"

#include "behaviour_tree/tasks/bt_action.hpp"
#include "behaviour_tree/tasks/bt_condition.hpp"
#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/composites/bt_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_random_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_random_sequence.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_fail.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_succeed.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"
#include "behaviour_tree/tasks/decorators/bt_probability.hpp"
#include "behaviour_tree/tasks/decorators/bt_repeat.hpp"

class BTGraphView : public godot::GraphEdit
{
    GDCLASS(BTGraphView, godot::GraphEdit)

private:
    godot::HashMap<godot::StringName, BTGraphNode*> task_name_to_node;

public:

    BTGraphView(){};
    ~BTGraphView(){};

    bool has_task_name(const godot::StringName& task_name) const;

    void create_task_node(const godot::StringName& task_name, const godot::StringName& task_class_name = BTTask::get_class_static());
    void set_task_title(const godot::StringName& task_name, const godot::String& task_title);
    void delete_task_node(const godot::StringName& task_name);
    void change_task_class_name(const godot::StringName& task_name, const godot::StringName& task_class_name);

    void connect_task_nodes(const godot::StringName& parent_task_name, const godot::StringName& child_task_name);
    void disconnect_task_nodes(const godot::StringName& parent_task_name, const godot::StringName& child_task_name);

    void set_node_position(const godot::StringName& task_name, godot::Vector2 position);

    _FORCE_INLINE_ BTGraphNode* get_graph_node(const godot::StringName& task_name) const
    {
        ERR_FAIL_COND_V_MSG(!(this->has_task_name(task_name)), nullptr, "BTGraphView has no node named: " + task_name + ".");
        return this->task_name_to_node[task_name];
    }

    godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> get_node_tree_map(const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const;

    godot::HashMap<BTGraphNode*, godot::Vector2> get_arranged_nodes_positions(const godot::StringName& root_task_name, const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const;
    void arrange_nodes(const godot::StringName& root_task_name, const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names);

protected:
    static void _bind_methods();
};

#endif /* BT_GRAPH_VIEW_HPP */