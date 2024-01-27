#include "bt_editor_plugin.hpp"

BTEditorPlugin::BTEditorPlugin()
{
    this->main_container = memnew(godot::HSplitContainer);

    this->graph_editor = memnew(godot::GraphEdit);
    this->graph_editor->set_h_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);
    this->graph_editor->set_v_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);

    this->button_continer = memnew(godot::VBoxContainer);

    this->add_new_node_button = memnew(godot::Button);
    this->add_new_node_button->set_text("Insert a node");
    this->add_new_node_button->connect("pressed", callable_mp(this, &BTEditorPlugin::add_new_node_button_pressed));

    this->button_continer->add_child(this->add_new_node_button);

    this->main_container->add_child(this->graph_editor);
    this->main_container->add_child(this->button_continer);

    this->bottom_panel_button = 
        add_control_to_bottom_panel(this->main_container,
                                    this->bottom_panel_button_name);


    this->_make_visible(false);
}

BTEditorPlugin::~BTEditorPlugin()
{
    
}

void BTEditorPlugin::set_behaviour_tree(BehaviourTree* new_tree)
{
    this->behaviour_tree = new_tree;
    //TODO:
    //this->clear_graph_nodes();
    //this->create_default_graph();

}
BTGraphNode* BTEditorPlugin::new_bt_graph_node()
{
    BTGraphNode* bt_graph_node = memnew(BTGraphNode);
    godot::Ref<BTTask> bt_task = godot::Ref<BTTask>(memnew(BTTask));

    bt_graph_node->set_task(bt_task);

    bt_graph_node->set_resizable(false);
    bt_graph_node->set_custom_minimum_size(godot::Size2(200, 10));

    bt_graph_node->set_position_offset(godot::Vector2(100, 100));
}

void BTEditorPlugin::add_new_node_button_pressed()
{
    BTGraphNode* bt_graph_node = BTEditorPlugin::new_bt_graph_node();
    bt_graph_node->set_graph_editor(this->graph_editor);
    bt_graph_node->set_title(godot::String("[") + godot::String("New node") + godot::String("]"));

    // this does nothing right now.
    bt_graph_node->set_name(godot::String("1"));
    
    //TODO:
    //bt_graph_node->connect("dragged", callable_mp(this, &BTEditorPlugin::_node_dragged).bind(id));
    //bt_graph_node->connect("node_selected", callable_mp(this, &BTEditorPlugin::_node_selected).bind(id));
    //bt_graph_node->connect("node_deselected", callable_mp(this, &BTEditorPlugin::_node_deselected).bind(id));

    this->graph_editor->add_child(bt_graph_node);


    this->behaviour_tree->add_task(bt_graph_node->get_task());
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
        if (behaviour_tree != this->behaviour_tree)
        {
            this->set_behaviour_tree(behaviour_tree);
        }
        else
        {
            // do nothing for now
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

}