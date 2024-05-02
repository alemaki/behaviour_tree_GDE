#ifndef BT_GRAPH_EDIT_HPP
#define BT_GRAPH_EDIT_HPP

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/editor_undo_redo_manager.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/popup_menu.hpp>
#include <godot_cpp/templates/hash_map.hpp>

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

class BTGraphEditor : public godot::Object
{
    GDCLASS(BTGraphEditor, godot::Object)

    friend BTEditorPlugin;

private:
    godot::EditorPlugin* editor_plugin;
    godot::GraphEdit* graph_edit;
    BehaviourTree* behaviour_tree;
    
    godot::HashMap<godot::StringName, BTGraphNode*> name_to_node;
    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*> task_to_node;

    struct DragOperation
    {
        godot::Vector2 from_position;
        godot::Vector2 to_position;
        BTGraphNode* node;
    };
    godot::Vector<DragOperation> drag_buffer;
    bool drag_called;

    godot::LineEdit* rename_edit;
    godot::LineEdit* path_edit;
    godot::PopupMenu* main_popup_menu;
    godot::PopupMenu* task_type_popup_menu;

    godot::PopupMenu* action_condition_type_popup_menu;

    BTGraphNode* last_double_clicked_node;
    BTGraphNode* last_right_clicked_node;

    godot::Vector<godot::StringName> composite_names;
    godot::Vector<godot::StringName> decorator_names;

private:
    /* Setup Methods */
    void _setup_graph_edit();
    void _setup_task_names();
    void _setup_rename_edit();
    void _setup_path_edit();
    void _setup_popup_menu();
    void _fill_action_condition_type_popup_menu(const godot::StringName& action_condition);

    /* Utility Methods */
    void connect_graph_node_signals(BTGraphNode* node);
    BTGraphNode* new_graph_node();
    BTGraphNodeSubtree* new_graph_node_subtree();
    BTGraphNode* new_bt_graph_node_from_task(godot::Ref<BTTask> bt_task);
    BTGraphNodeSubtree* new_bt_graph_node_subtree_from_task(godot::Ref<BTSubtree> bt_subtree);
    godot::Array get_graph_nodes();
    godot::Array get_sorted_by_y_children_of_parent(BTGraphNode* parent_graph_node);
    int get_node_insert_index_by_y_in_children(BTGraphNode* parent_graph_node, BTGraphNode* graph_node);
    void _extract_node_levels_into_stack(BTGraphNode* root_node, godot::Vector<godot::Pair<BTGraphNode*, int>>& stack, int current_level = 0);

    /* Node Management */
    void insert_node(BTGraphNode* bt_graph_node);
    void erase_node(BTGraphNode* bt_graph_node);
    void delete_nodes(const godot::Vector<BTGraphNode*>& nodes_to_delete);
    void clear_graph_nodes();
    void create_default_graph_nodes();
    void set_root_node(BTGraphNode* new_root_node);
    void arrange_nodes();
    void evaluate_root_node();

    /* Drag and Drop */
    void _node_dragged(const godot::Vector2 &_from, const godot::Vector2 &_to, BTGraphNode *node);
    void _move_nodes();

    /* Event Handlers */
    void _add_new_node_button_pressed();
    void _add_new_subtree_node_button_pressed();
    void _arrange_nodes_button_pressed();
    void _on_rename_edit_text_submitted(const godot::String& new_text);
    void _on_rename_edit_focus_exited();
    void _on_path_edit_text_submitted(const godot::String& new_path);
    void _on_path_edit_focus_exited();
    void _on_node_selected(BTGraphNode* selected_node);
    void _on_node_deselected(BTGraphNode* deselected_node);
    void _on_node_double_clicked(BTGraphNode* clicked_node);
    void _on_node_right_clicked(BTGraphNode* clicked_node);
    void _on_node_subtree_double_clicked(BTGraphNodeSubtree* clicked_node);
    void _on_node_subtree_right_clicked(BTGraphNodeSubtree* clicked_node);
    void _on_main_popup_menu_item_selected(int id);
    void _on_task_type_popup_menu_item_selected(int id);
    void _on_action_condition_type_popup_menu_item_selected(int id);
    void _on_action_condition_type_popup_menu_show(const godot::StringName& action_condition);
    void _delete_nodes_request(godot::TypedArray<godot::StringName> _nodes_to_delete);

    /* Connection Handling */
    void connection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port);
    void disconnection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port);

    /* Task Management */
    void change_task_type(const godot::StringName& class_name, BTGraphNode* node);

    /* Getters and Setters */
    void set_editor_plugin(godot::EditorPlugin* editor_plugin);
    _FORCE_INLINE_ godot::GraphEdit* get_graph_edit() const
    {
        return this->graph_edit;
        }
    void set_behaviour_tree(BehaviourTree* new_tree);
    _FORCE_INLINE_ BehaviourTree* get_behaviour_tree() const
    {
        return this->behaviour_tree; 
    }

public:
    BTGraphEditor();
    ~BTGraphEditor();

protected:
    static void _bind_methods();
};

#endif /* BT_GRAPH_EDIT_HPP */