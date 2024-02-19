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
#include "behaviour_tree/ui/bt_graph_editor.hpp"

class BTEditorPlugin : public godot::EditorPlugin
{
    GDCLASS(BTEditorPlugin, godot::EditorPlugin)

private:
    const godot::String plugin_name = "BTEditorPlugin";
    const godot::String bottom_panel_button_name = "Behaviour tree";
    
    godot::VBoxContainer* button_continer;
    godot::Button* add_new_node_button;
    godot::Button* arrange_nodes_button;
    godot::Button* set_root_button;
    godot::Button* clear_nodes_button;

    godot::Button* bottom_panel_button;
    godot::HSplitContainer* main_container;
    BTGraphEditor* graph_editor;

private:
    void set_graph_editor(BTGraphEditor* graph_editor);
    _FORCE_INLINE_ BTGraphEditor* get_graph_editor()
    {
        return this->graph_editor;
    }

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