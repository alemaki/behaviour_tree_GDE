#ifndef BT_GRAPH_EDIT_HPP
#define BT_GRAPH_EDIT_HPP

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/editor_undo_redo_manager.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/popup_menu.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/hash_set.hpp>

#include "behaviour_tree/ui/bt_graph_view.hpp"

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

#ifndef BT_EDITOR_PLUGIN_FORWARD
#define BT_EDITOR_PLUGIN_FORWARD
class BTEditorPlugin;
#endif /* BT_EDITOR_PLUGIN_FORWARD */

/*  For testing purposes, BTGraphEditor will be a node for now. 
    For some reason the memdelete(BTGraphEditor) when BTGraphEditor is an object causes segfault in the engine.
    ->queue_free() on the nodes manages to escape that.  TODO: investigate later. */
class BTGraphEditor : public godot::Node
{
    GDCLASS(BTGraphEditor, godot::Node)

    friend BTEditorPlugin;

private:
    BTGraphView* graph_view = nullptr;
    godot::EditorPlugin* editor_plugin = nullptr;
    BehaviourTree* behaviour_tree = nullptr;

    godot::LineEdit* rename_edit = nullptr;
    godot::LineEdit* path_edit = nullptr;
    godot::PopupMenu* main_popup_menu = nullptr;
    godot::PopupMenu* task_type_popup_menu = nullptr;

    godot::PopupMenu* action_condition_type_popup_menu = nullptr;

    godot::StringName last_double_clicked_node = "";
    godot::StringName last_right_clicked_node = "";

    godot::Vector<godot::StringName> composite_names;
    godot::Vector<godot::StringName> decorator_names;

    struct DragOperation
    {
        godot::Vector2 from_position;
        godot::Vector2 to_position;
        godot::StringName task_name;
    };
    godot::Vector<DragOperation> drag_buffer;
    bool drag_called;

    godot::HashSet<BehaviourTree*> saved_trees;

public:
    BTGraphEditor();
    ~BTGraphEditor();

private:
    /* Setup Methods */
    void _setup_graph_view();
    void _setup_task_names();
    void _setup_rename_edit();
    void _setup_path_edit();
    void _setup_popup_menu();
    void _fill_action_condition_type_popup_menu(const godot::StringName& action_condition);

    /* Utility Methods */
    void connect_graph_node_signals(const godot::StringName& task_name);

    /* Node Management */
    void delete_nodes(const godot::Vector<StringName>& task_names_to_delete); /* refactored */
    void create_default_graph_nodes(); /* refactored */
    void set_root_node(const godot::StringName& task_name); /* refactored */
    void arrange_nodes(bool with_undo_redo = false); /* refactored */
    void color_root_node(); /* refactored */
    //void deselect_all_nodes();
    void save_tree(); /* refactored */
    void load_tree(); /* refactored */

    /* Copy-pasta Handling */
    void copy_nodes_request();
    void paste_nodes_request();
    void clear_copied_nodes();

    static godot::Vector<StringName> bttask_array_to_names(godot::Array children);
    
public:
    /* Connection Handling */
    void connection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port); /* refactored */
    void disconnection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port); /* refactored */

    /* Drag and Drop */
    void _node_dragged(const godot::Vector2 &_from, const godot::Vector2 &_to, const godot::StringName& task_name); /* refactored */
    void _move_nodes(); /* refactored */

    /* Event Handlers */
    void _add_new_node_button_pressed(); /* refactored */
    void _add_new_subtree_node_button_pressed(); /* refactored */
    void _arrange_nodes_button_pressed(); /* refactored */
    void _on_rename_edit_text_submitted(const godot::String& new_text);
    void _on_rename_edit_focus_exited();
    void _on_path_edit_text_submitted(const godot::String& new_path);
    void _on_path_edit_focus_exited();
    void _on_node_selected(const godot::StringName& task_name); /* refactored */
    void _on_node_deselected(const godot::StringName& task_name); /* refactored */
    void _on_node_double_clicked(const godot::StringName& task_name); /* refactored */
    void _on_node_right_clicked(const godot::StringName& task_name); /* refactored */
    void _on_node_subtree_double_clicked(const godot::StringName& task_name); /* refactored */
    void _on_node_subtree_right_clicked(const godot::StringName& task_name); /* refactored */
    void _on_main_popup_menu_item_selected(int id);
    void _on_task_type_popup_menu_item_selected(int id);
    void _on_action_condition_type_popup_menu_item_selected(int id);
    void _on_action_condition_type_popup_menu_show(const godot::StringName& action_condition);
    void _delete_nodes_request(godot::TypedArray<godot::StringName> _nodes_to_delete); /* refactored */
    void _change_task_type(const godot::StringName& class_name, const godot::StringName& task_name); /* refactored*/

    /* Getters and Setters */
    void set_editor_plugin(godot::EditorPlugin* editor_plugin);
    void set_behaviour_tree(BehaviourTree* new_tree); /* refactored */
    _FORCE_INLINE_ BehaviourTree* get_behaviour_tree() const
    {
        return this->behaviour_tree; 
    }

    _FORCE_INLINE_ BTGraphView* get_graph_view() const
    {
        return this->graph_view; 
    }

protected:
    static void _bind_methods();
};

#endif /* BT_GRAPH_EDIT_HPP */