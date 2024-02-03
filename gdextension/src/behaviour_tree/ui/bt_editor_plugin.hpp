#ifndef BT_EDITOR_PLUGIN_HPP
#define BT_EDITOR_PLUGIN_HPP

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/editor_undo_redo_manager.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/h_split_container.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include "behaviour_tree/behaviour_tree.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"

class BTEditorPlugin : public godot::EditorPlugin
{
    GDCLASS(BTEditorPlugin, godot::EditorPlugin)

private:
    const godot::String plugin_name = "BTEditorPlugin";
    const godot::String bottom_panel_button_name = "Behaviour tree";

    godot::VBoxContainer* button_continer;
    godot::Button* add_new_node_button;
    godot::Button* clear_nodes_button;

    godot::Button* bottom_panel_button;
    godot::HSplitContainer* main_container;
    godot::GraphEdit* graph_editor;

    BehaviourTree* behaviour_tree;

    godot::HashMap<godot::StringName, BTGraphNode*> node_map;

private: 
    static BTGraphNode* new_bt_graph_node();
    static BTGraphNode* new_bt_graph_node_from_task(godot::Ref<BTTask> bt_task);

    void set_behaviour_tree(BehaviourTree* new_tree);
    void add_node_method(int id, BTGraphNode* bt_graph_node);
    void remove_node_method(int id, BTGraphNode* bt_graph_node);
    void add_new_node_button_pressed();
    void clear_graph_button_pressed();

    void connection_request(godot::StringName from_node, int from_port, godot::StringName to_node, int to_port);

    void clear_graph_nodes();
    void create_default_graph_nodes();
public:
    BTEditorPlugin();
    ~BTEditorPlugin();
    virtual void _make_visible(bool visible) override;

    virtual void _edit(Object *object) override;
    virtual bool _handles(Object *object) const override;


protected:
    static void _bind_methods();
};

#endif //BT_EDITOR_PLUGIN_HPP