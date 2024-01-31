#include "bt_editor_plugin.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
BTEditorPlugin::BTEditorPlugin()
{
    this->main_container = memnew(godot::HSplitContainer);

    this->graph_editor = memnew(godot::GraphEdit);
    this->graph_editor->set_h_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);
    this->graph_editor->set_v_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);

    this->button_continer = memnew(godot::VBoxContainer);

    this->add_new_node_button = memnew(godot::Button);
    this->add_new_node_button->set_text("Insert node");
    this->add_new_node_button->connect("pressed", callable_mp(this, &BTEditorPlugin::add_new_node_button_pressed));

    this->clear_nodes_button = memnew(godot::Button);
    this->clear_nodes_button->set_text("Clear nodes");
    this->clear_nodes_button->connect("pressed", callable_mp(this, &BTEditorPlugin::clear_graph_button_pressed));

    this->button_continer->add_child(this->add_new_node_button);
    this->button_continer->add_child(this->clear_nodes_button);

    this->main_container->add_child(this->graph_editor);
    this->main_container->add_child(this->button_continer);

    this->bottom_panel_button =
        add_control_to_bottom_panel(this->main_container,
                                    this->bottom_panel_button_name);

    this->_make_visible(false);

    this->graph_editor->connect("connection_request", callable_mp(this, &BTEditorPlugin::connection_request));
}

BTEditorPlugin::~BTEditorPlugin()
{

}

void BTEditorPlugin::connection_request(godot::StringName from_node, int from_port, godot::StringName to_node, int to_port)
{
    
}

void BTEditorPlugin::clear_graph_button_pressed()
{
    this->clear_graph_nodes();
    this->behaviour_tree->clear_tasks();
}

void BTEditorPlugin::clear_graph_nodes()
{
    godot::TypedArray<godot::Node> children = this->graph_editor->get_children();
    for (int i = 0, size = children.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(children[i]);

        if (bt_graph_node != nullptr)
        {
            this->graph_editor->remove_child(bt_graph_node);
            this->behaviour_tree->remove_task(bt_graph_node->get_task());
            bt_graph_node->queue_free();
        }
    }
}

void BTEditorPlugin::create_default_graph_nodes()
{
    godot::Array tasks = this->behaviour_tree->get_tasks();

    for (int i = 0, size = tasks.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = new_bt_graph_node_from_task(godot::Ref<BTTask>(tasks[i]));
        bt_graph_node->set_graph_editor(this->graph_editor);
        bt_graph_node->set_title(godot::String("[") + godot::String("New node") + godot::String("]"));
        bt_graph_node->set_name(godot::String("1"));
        this->graph_editor->add_child(bt_graph_node);
    }
}

void BTEditorPlugin::set_behaviour_tree(BehaviourTree* new_tree)
{
    this->behaviour_tree = new_tree;
    this->clear_graph_nodes();
    this->create_default_graph_nodes();
}

BTGraphNode* BTEditorPlugin::new_bt_graph_node_from_task(godot::Ref<BTTask> bt_task)
{
    BTGraphNode* bt_graph_node = memnew(BTGraphNode);

    bt_graph_node->set_task(bt_task);

    bt_graph_node->set_resizable(false);
    bt_graph_node->set_custom_minimum_size(godot::Size2(200, 10));

    bt_graph_node->set_position_offset(godot::Vector2(100, 100));

    return bt_graph_node;
}

BTGraphNode* BTEditorPlugin::new_bt_graph_node()
{
    godot::Ref<BTTask> bt_task = godot::Ref<BTTask>(memnew(BTTask));
    BTGraphNode* bt_graph_node = BTEditorPlugin::new_bt_graph_node_from_task(bt_task);

    return bt_graph_node;
}

void BTEditorPlugin::add_new_node_button_pressed()
{
    BTGraphNode* bt_graph_node = BTEditorPlugin::new_bt_graph_node();

    bt_graph_node->set_graph_editor(this->graph_editor);
    bt_graph_node->set_title(godot::String("[") + godot::String("New node") + godot::String("]"));

    //TODO:
    //bt_graph_node->connect("dragged", callable_mp(this, &BTEditorPlugin::_node_dragged).bind(id));
    //bt_graph_node->connect("node_selected", callable_mp(this, &BTEditorPlugin::_node_selected).bind(id));
    //bt_graph_node->connect("node_deselected", callable_mp(this, &BTEditorPlugin::_node_deselected).bind(id));

    godot::EditorUndoRedoManager* undo_redo_manager = this->get_undo_redo();

    undo_redo_manager->create_action("Add a node to the behaviour tree.");
    undo_redo_manager->add_do_method(this->behaviour_tree, "add_task", bt_graph_node->get_task());
    undo_redo_manager->add_do_method(this->graph_editor, "add_child", bt_graph_node);

    undo_redo_manager->add_undo_method(this->graph_editor, "remove_child", bt_graph_node);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task", bt_graph_node->get_task());

    undo_redo_manager->commit_action();

    godot::String name = godot::itos(this->behaviour_tree->get_task_id(bt_graph_node->get_task()));
    bt_graph_node->set_name(name);
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