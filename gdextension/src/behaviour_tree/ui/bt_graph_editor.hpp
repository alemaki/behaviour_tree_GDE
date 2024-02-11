#ifndef BT_GRAPH_EDIT_HPP
#define BT_GRAPH_EDIT_HPP

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/editor_undo_redo_manager.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include "behaviour_tree/behaviour_tree.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"

#ifndef BT_EDITOR_PLUGIN_FORWARD
#define BT_EDITOR_PLUGIN_FORWARD
/* forward */
class BTEditorPlugin;
#endif //BT_EDITOR_PLUGIN_FORWARD


class BTGraphEditor : public godot::RefCounted
{
    GDCLASS(BTGraphEditor, godot::RefCounted)

    friend BTEditorPlugin;

private:
    godot::EditorPlugin* editor_plugin;

    godot::GraphEdit* graph_editor;

    BehaviourTree* behaviour_tree;
    godot::HashMap<godot::StringName, BTGraphNode*> node_map;

    struct DragOperation
    {
        godot::Vector2 from_position;
        godot::Vector2 to_position;
        godot::StringName node_name;
    };
    godot::Vector<DragOperation> drag_buffer;
    bool drag_called;
    bool arranging_nodes;

private:
    void set_editor_plugin(godot::EditorPlugin* editor_plugin);
    _FORCE_INLINE_ godot::GraphEdit* get_graph_edit()
    {
        return this->graph_editor;
    }
    BTGraphNode* new_bt_graph_node();
    BTGraphNode* new_bt_graph_node_from_task(godot::Ref<BTTask> bt_task);

    godot::Array get_graph_nodes();

    void set_behaviour_tree(BehaviourTree* new_tree);
    _FORCE_INLINE_ BehaviourTree* get_behaviour_tree()
    {
        return this->behaviour_tree;
    }
    void insert_node(BTGraphNode* bt_graph_node);
    void erase_node(BTGraphNode* bt_graph_node);
    int get_node_position_in_children(BTGraphNode* graph_node, BTGraphNode* parent_graph_node);
    void _add_new_node_button_pressed();
    void _arrange_nodes_button_pressed();
    void _clear_graph_button_pressed();
    void _node_dragged(const godot::Vector2 &_from, const godot::Vector2 &_to, godot::StringName node_name);
    void _move_nodes();
    void connection_request(godot::StringName from_node, int from_port, godot::StringName to_node, int to_port);

    void clear_graph_nodes();
    void create_default_graph_nodes();
    void _extract_node_levels_into_stack(BTGraphNode* root_node, godot::Vector<godot::Pair<BTGraphNode*, int>>& stack, godot::HashMap<godot::Ref<BTTask>, BTGraphNode*>& task_to_node,  int current_level = 0);
    void arrange_nodes();

public:
    BTGraphEditor();
    ~BTGraphEditor();


protected:
    static void _bind_methods();
};

#endif //BT_GRAPH_EDIT_HPP