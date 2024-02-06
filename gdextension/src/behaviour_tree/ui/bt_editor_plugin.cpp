#include "bt_editor_plugin.hpp"
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

BTEditorPlugin::BTEditorPlugin()
{
    this->main_container = memnew(godot::HSplitContainer);

    this->graph_editor = memnew(godot::GraphEdit);
    this->graph_editor->set_h_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);
    this->graph_editor->set_v_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);

    this->button_continer = memnew(godot::VBoxContainer);

    this->add_new_node_button = memnew(godot::Button);
    this->add_new_node_button->set_text("Insert node");
    this->add_new_node_button->connect("pressed", callable_mp(this, &BTEditorPlugin::_add_new_node_button_pressed));

    this->arrange_nodes_button = memnew(godot::Button);
    this->arrange_nodes_button->set_text("Arrange nodes");
    this->arrange_nodes_button->connect("pressed", callable_mp(this, &BTEditorPlugin::_arrange_nodes_button_pressed));

    this->clear_nodes_button = memnew(godot::Button);
    this->clear_nodes_button->set_text("Clear nodes");
    this->clear_nodes_button->connect("pressed", callable_mp(this, &BTEditorPlugin::_clear_graph_button_pressed));

    this->button_continer->add_child(this->add_new_node_button);
    this->button_continer->add_child(this->arrange_nodes_button);
    this->button_continer->add_child(this->clear_nodes_button);

    this->main_container->add_child(this->graph_editor);
    this->main_container->add_child(this->button_continer);

    this->bottom_panel_button =
        add_control_to_bottom_panel(this->main_container,
                                    this->bottom_panel_button_name);

    this->_make_visible(false);

    this->graph_editor->connect("connection_request", callable_mp(this, &BTEditorPlugin::connection_request));

    this->graph_editor->add_valid_connection_type(1, 0);
}

BTEditorPlugin::~BTEditorPlugin()
{
    
}

void BTEditorPlugin::connection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port)
{

    ERR_FAIL_COND_MSG(!(this->node_map.has(_from_node)), "From_node doesn't exist.");
    ERR_FAIL_COND_MSG(!(this->node_map.has(_to_node)), "To_node doesn't exist.");

    BTGraphNode* from_node = this->node_map[_from_node];
    BTGraphNode* to_node = this->node_map[_to_node];

    bool can_connect = this->behaviour_tree->can_connect(from_node->get_task(), to_node->get_task());
    if (can_connect)
    {
        godot::EditorUndoRedoManager* undo_redo_manager = this->get_undo_redo();
        
        undo_redo_manager->create_action("Create a connection between nodes.");

        undo_redo_manager->add_do_method(this->behaviour_tree, "connect_tasks", from_node->get_task(), to_node->get_task(), 0);
        undo_redo_manager->add_do_method(this->graph_editor, "connect_node", _from_node, from_port, _to_node, to_port);
        
        undo_redo_manager->add_undo_method(this->graph_editor, "disconnect_node", _from_node, from_port, _to_node, to_port);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "disconnect_tasks", from_node->get_task(), to_node->get_task());

        undo_redo_manager->commit_action();
    }
}

void BTEditorPlugin::_clear_graph_button_pressed()
{
    this->clear_graph_nodes();
    this->behaviour_tree->clear_tasks();
    this->node_map.clear();
}

void BTEditorPlugin::clear_graph_nodes()
{
    this->graph_editor->clear_connections();

    godot::TypedArray<godot::Node> children = this->graph_editor->get_children();
    for (int i = 0, size = children.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(children[i]);

        if (bt_graph_node != nullptr)
        {
            this->graph_editor->remove_child(bt_graph_node);
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

        int id = this->behaviour_tree->get_task_id(godot::Ref<BTTask>(tasks[i]));
        bt_graph_node->set_title(godot::String("[") + godot::itos(id) + godot::String("]"));
        bt_graph_node->set_name(godot::itos(id));
        this->graph_editor->add_child(bt_graph_node);
    }
    /* TODO: Make connections */
}

godot::Array BTEditorPlugin::get_graph_nodes()
{
    godot::Array result;
    for (const godot::KeyValue<godot::StringName, BTGraphNode*>& element : this->node_map)
    {
        if (element.value == nullptr)
        {
            godot::UtilityFunctions::printerr("Invalid node from reading. Key: " + element.key);
            continue;
        }
        result.push_back(element.value);
    }
    return result;
}

void BTEditorPlugin::_extract_node_levels_into_stack(
    BTGraphNode* root_node, 
    godot::Vector<godot::Pair<BTGraphNode*, int>>& stack, 
    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*>& task_to_node, 
    int current_level)
{
    stack.push_back(godot::Pair<BTGraphNode*, int>(root_node, current_level));

    godot::Array children = root_node->get_task()->get_children();

    for (int i = 0, size = children.size(); i < size; i++)
    {
        BTGraphNode* child_node = Object::cast_to<BTGraphNode>(task_to_node[children[i]]);
        this->_extract_node_levels_into_stack(child_node, stack, task_to_node, current_level + 1);
    }
}

void BTEditorPlugin::arrange_nodes()
{
    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*> task_to_node;
    godot::Array bt_graph_nodes = this->get_graph_nodes();
    if (bt_graph_nodes.size() == 0)
    {
        return;
    }
    for (int i = 0, size = bt_graph_nodes.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(bt_graph_nodes[i]);
        task_to_node[bt_graph_node->get_task()] = bt_graph_node;
    }
    BTGraphNode* root_node = task_to_node[this->behaviour_tree->get_root_task()];

    godot::Vector<godot::Pair<BTGraphNode*, int>> stack;
    this->_extract_node_levels_into_stack(root_node, stack, task_to_node);

    godot::HashMap<int, int> level_to_node_count;
    godot::Vector2 root_node_position = root_node->get_position_offset();
    godot::Vector2 node_padding = godot::Vector2(130, 40);
    int current_index = stack.size() - 1;

    while (current_index >= 0)
    {
        godot::Pair<BTGraphNode*, int> pair_node_level = stack[current_index];

        godot::UtilityFunctions::print(level_to_node_count[pair_node_level.second]);
        godot::Vector2 position = godot::Vector2(0, 0);
        position.x += node_padding.x*pair_node_level.second;
        position.y += node_padding.y*level_to_node_count[pair_node_level.second];
        position += root_node_position;

        pair_node_level.first->set_position_offset(position);

        level_to_node_count[pair_node_level.second]++;
        current_index--;
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

    return bt_graph_node;
}

BTGraphNode* BTEditorPlugin::new_bt_graph_node()
{
    godot::Ref<BTTask> bt_task = godot::Ref<BTTask>(memnew(BTTask));
    BTGraphNode* bt_graph_node = BTEditorPlugin::new_bt_graph_node_from_task(bt_task);

    return bt_graph_node;
}


void BTEditorPlugin::add_node_method(int id, BTGraphNode* bt_graph_node)
{
    this->behaviour_tree->add_task(id, bt_graph_node->get_task());
    this->graph_editor->add_child(bt_graph_node);
    this->node_map.insert(bt_graph_node->get_name(), bt_graph_node);
}

void BTEditorPlugin::remove_node_method(int id, BTGraphNode* bt_graph_node)
{
    this->node_map.erase(bt_graph_node->get_name());
    this->graph_editor->remove_child(bt_graph_node);
    this->behaviour_tree->remove_task(id);
}

void BTEditorPlugin::_add_new_node_button_pressed()
{
    BTGraphNode* bt_graph_node = BTEditorPlugin::new_bt_graph_node();

    bt_graph_node->set_graph_editor(this->graph_editor);

    int id = this->behaviour_tree->get_valid_id();
    bt_graph_node->set_title(godot::String("[") + godot::itos(id) + godot::String("]"));
    bt_graph_node->set_name(godot::itos(id));

    /* TODO:
    /* bt_graph_node->connect("dragged", callable_mp(this, &BTEditorPlugin::_node_dragged).bind(id));
    /* bt_graph_node->connect("node_selected", callable_mp(this, &BTEditorPlugin::_node_selected).bind(id));
    /* bt_graph_node->connect("node_deselected", callable_mp(this, &BTEditorPlugin::_node_deselected).bind(id)); */

    godot::EditorUndoRedoManager* undo_redo_manager = this->get_undo_redo();

    undo_redo_manager->create_action("Add a node.");

    undo_redo_manager->add_do_method(this, "add_node_method", id, bt_graph_node);
    undo_redo_manager->add_undo_method(this, "remove_node_method", id, bt_graph_node);

    undo_redo_manager->commit_action();

}

void BTEditorPlugin::_arrange_nodes_button_pressed()
{
    this->arrange_nodes();
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

    ClassDB::bind_method(D_METHOD("add_node_method", "id", "bt_graph_node"), &BTEditorPlugin::add_node_method);
    ClassDB::bind_method(D_METHOD("remove_node_method", "id", "bt_graph_node"), &BTEditorPlugin::remove_node_method);

}