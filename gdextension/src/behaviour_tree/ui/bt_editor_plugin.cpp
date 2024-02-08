#include "bt_editor_plugin.hpp"
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

BTEditorPlugin::BTEditorPlugin()
{
    this->graph_editor = godot::Ref<BTGraphEditor>(memnew(BTGraphEditor));
    this->graph_editor->set_editor_plugin(this);

    this->main_container = memnew(godot::HSplitContainer);

    this->button_continer = memnew(godot::VBoxContainer);

    this->add_new_node_button = memnew(godot::Button);
    this->add_new_node_button->set_text("Insert node");
    this->add_new_node_button->connect("pressed", callable_mp(this->graph_editor.ptr(), &BTGraphEditor::_add_new_node_button_pressed));

    this->arrange_nodes_button = memnew(godot::Button);
    this->arrange_nodes_button->set_text("Arrange nodes");
    this->arrange_nodes_button->connect("pressed", callable_mp(this->graph_editor.ptr(), &BTGraphEditor::_arrange_nodes_button_pressed));

    this->clear_nodes_button = memnew(godot::Button);
    this->clear_nodes_button->set_text("Clear nodes");
    this->clear_nodes_button->connect("pressed", callable_mp(this->graph_editor.ptr(), &BTGraphEditor::_clear_graph_button_pressed));

    this->button_continer->add_child(this->add_new_node_button);
    this->button_continer->add_child(this->arrange_nodes_button);
    this->button_continer->add_child(this->clear_nodes_button);

    this->main_container->add_child(this->graph_editor->get_graph_editor());
    this->main_container->add_child(this->button_continer);

    this->bottom_panel_button =
        add_control_to_bottom_panel(this->main_container,
                                    this->bottom_panel_button_name);

    this->_make_visible(false);
}

BTEditorPlugin::~BTEditorPlugin()
{
    
}
void BTEditorPlugin::_make_visible(bool visible)
{
    if (visible)
    {
        this->bottom_panel_button->show();
        this->make_bottom_panel_item_visible(this->main_container);
    }
    else
    {
        if (this->main_container->is_visible_in_tree())
        {
            this->hide_bottom_panel();
        }
        this->bottom_panel_button->hide();
    }
}

void BTEditorPlugin::_edit(Object* object)
{
    BehaviourTree* behaviour_tree = godot::Object::cast_to<BehaviourTree>(object);
    if (behaviour_tree != nullptr)
    {
        if (behaviour_tree != this->graph_editor->behaviour_tree)
        {
            this->graph_editor->set_behaviour_tree(behaviour_tree);
        }
        else
        {
            /* do nothing for now */
        }
    }
    return;
}

bool BTEditorPlugin::_handles(Object* object) const
{
    bool visible = ((godot::Object::cast_to<BehaviourTree>(object) != nullptr) ||
                   (godot::Object::cast_to<BTTask>(object)!= nullptr));

    return visible;
}

void BTEditorPlugin::_bind_methods()
{
    using namespace godot;
}