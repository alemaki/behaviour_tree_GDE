#include "bt_graph_editor.hpp"
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

BTGraphEditor::BTGraphEditor()
{
    this->graph_edit = memnew(godot::GraphEdit);
    this->graph_edit->set_h_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);
    this->graph_edit->set_v_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);

    this->graph_edit->call_deferred("connect", "connection_request", callable_mp(this, &BTGraphEditor::connection_request));
    this->graph_edit->call_deferred("connect", "disconnection_request", callable_mp(this, &BTGraphEditor::disconnection_request));
    this->graph_edit->call_deferred("connect", "delete_nodes_request", callable_mp(this, &BTGraphEditor::_delete_nodes_request));

    /* right to left */
    this->graph_edit->add_valid_connection_type(1, 0);
    this->graph_edit->add_valid_left_disconnect_type(0);

    this->drag_called = false;

    this->graph_edit->set_right_disconnects(true);

    this->_setup_rename_edit();
    this->_setup_popup_menu();
}

BTGraphEditor:: ~BTGraphEditor()
{
    
}

/* Setup Methods */
void BTGraphEditor::_setup_rename_edit()
{
    ERR_FAIL_COND(this->graph_edit == nullptr);

    this->rename_edit = memnew(godot::LineEdit);
    this->rename_edit->set_visible(false);
    this->graph_edit->add_child(this->rename_edit);

    this->rename_edit->call_deferred("connect", "text_submitted", callable_mp(this, &BTGraphEditor::_on_rename_edit_text_submitted));
    this->rename_edit->call_deferred("connect", "focus_exited", callable_mp(this, &BTGraphEditor::_on_rename_edit_focus_exited));
}

void BTGraphEditor::_setup_popup_menu()
{
    ERR_FAIL_COND(this->graph_edit == nullptr);

    this->main_popup_menu = memnew(godot::PopupMenu);
    this->main_popup_menu->set_name("MainPopupMenu");
    this->main_popup_menu->set_allow_search(false);

    this->task_type_popup_menu = memnew(godot::PopupMenu);
    this->task_type_popup_menu->set_name("TaskTypePopupMenu");

    godot::Vector<godot::StringName> task_names = 
    {
        BTTask::get_class_static(),
        BTSelector::get_class_static(),
        BTSequence::get_class_static(),
        BTRandomSelector::get_class_static(),
        BTRandomSequence::get_class_static(),
        BTAlwaysFail::get_class_static(),
        BTAlwaysSucceed::get_class_static(),
        BTInvert::get_class_static(),
        BTProbability::get_class_static(),
        BTRepeat::get_class_static(),
        BTAction::get_class_static(),
    };

    for (const godot::StringName& task_name : task_names)
    {
        this->task_type_popup_menu->add_radio_check_item(task_name);
    }

    this->main_popup_menu->add_child(this->task_type_popup_menu);

    this->main_popup_menu->add_item("Change name");
    this->main_popup_menu->add_item("Set root");
    this->main_popup_menu->add_item("Delete");
    this->main_popup_menu->add_submenu_item("Task type", "TaskTypePopupMenu");

    this->main_popup_menu->set_transient(true);
    this->task_type_popup_menu->set_transient(true);

    this->graph_edit->add_child(this->main_popup_menu);

    this->task_type_popup_menu->call_deferred("connect", "id_pressed", callable_mp(this, &BTGraphEditor::_on_task_type_popup_menu_item_selected));
    this->main_popup_menu->call_deferred("connect", "id_pressed", callable_mp(this, &BTGraphEditor::_on_main_popup_menu_item_selected));
    this->main_popup_menu->call_deferred("connect", "close_requested", callable_mp(this, &BTGraphEditor::_on_main_popup_menu_close_requested));
}

/* Utility Methods */

BTGraphNode* BTGraphEditor::new_bt_graph_node()
{
    godot::Ref<BTTask> bt_task = godot::Ref<BTTask>(memnew(BTTask));
    BTGraphNode* bt_graph_node = this->new_bt_graph_node_from_task(bt_task);

    return bt_graph_node;
}

BTGraphNode* BTGraphEditor::new_bt_graph_node_from_task(godot::Ref<BTTask> bt_task)
{
    BTGraphNode* bt_graph_node = memnew(BTGraphNode);
    bt_graph_node->set_task(bt_task);
    bt_graph_node->set_graph_edit(this->graph_edit);

    return bt_graph_node;
}

godot::Array BTGraphEditor::get_graph_nodes()
{
    godot::Array result;
    for (const godot::KeyValue<godot::StringName, BTGraphNode*>& element : this->name_to_node)
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

godot::Array BTGraphEditor::get_sorted_by_y_children_of_parent(BTGraphNode* parent_graph_node)
{
    godot::Ref<BTTask> parent_task = parent_graph_node->get_task();

    godot::Array children = parent_graph_node->get_task()->get_children();
    int size = children.size();
    godot::Vector<BTGraphNode*> nodes;
    nodes.resize(size);
    for (int i = 0; i < size; i++)
    {
        BTGraphNode* graph_node = this->task_to_node[godot::Ref<BTTask>(children[i])];
        nodes.set(i, graph_node);
    }

    /* TODO: optimise bubble sort */
    for (int i = 0; i < size; i++)
    {
        for (int j = i; j < size; j++)
        {
            if (nodes[i]->get_position_offset().y > nodes[j]->get_position_offset().y)
            {
                /* TODO: Find swap for vector, since read-only */
                BTGraphNode* temp = nodes[i];
                nodes.set(i, nodes[j]);
                nodes.set(j, temp);

                SWAP(children[i], children[j]);
            }
        }
    }

    return children;
}

int BTGraphEditor::get_node_insert_index_by_y_in_children(BTGraphNode* parent_graph_node, BTGraphNode* graph_node)
{
    godot::Ref<BTTask> task = graph_node->get_task();
    godot::Ref<BTTask> parent_task = parent_graph_node->get_task();

    if (parent_task->has_child(task))
    {
        godot::UtilityFunctions::printerr("Error: No way to insert - parent already has that task!");
        return -1;
    }

    godot::Array all_children = parent_task->get_children();
    int size = all_children.size();

    if (size == 0)
    {
        return 0;
    }

    for (int i = 0; i < size - 1; i++)
    {
        BTGraphNode* node1 = this->task_to_node[all_children[i]];
        BTGraphNode* node2 = this->task_to_node[all_children[i + 1]];

        if (graph_node->get_position_offset().y >= node1->get_position_offset().y &&
            graph_node->get_position_offset().y <= node2->get_position_offset().y)
        {
            return i + 1;
        }
    }
    BTGraphNode* first_child_node = this->task_to_node[all_children[0]];
    if (graph_node->get_position_offset().y < first_child_node->get_position_offset().y)
    {
        return 0;
    }
    
    return size;
}

void BTGraphEditor::_extract_node_levels_into_stack(BTGraphNode* root_node, godot::Vector<godot::Pair<BTGraphNode*, int>>& stack, int current_level)
{
    stack.push_back(godot::Pair<BTGraphNode*, int>(root_node, current_level));

    godot::Array children = root_node->get_task()->get_children();
    /* reverse so the first node to exit is the first child */ 
    for (int size = children.size(), i = size - 1; i >= 0; i--)
    {
        BTGraphNode* child_node = Object::cast_to<BTGraphNode>(this->task_to_node[children[i]]);
        if (child_node == nullptr)
        {
            godot::UtilityFunctions::printerr("Invalid node from reading. Key: " + godot::String(godot::Variant(children[i])));
            continue;
        }
        this->_extract_node_levels_into_stack(child_node, stack, current_level + 1);
    }
}


/* Node Management */

void BTGraphEditor::insert_node(BTGraphNode* bt_graph_node)
{
    ERR_FAIL_COND(bt_graph_node == nullptr);

    this->name_to_node.insert(bt_graph_node->get_name(), bt_graph_node);
    this->task_to_node.insert(bt_graph_node->get_task(), bt_graph_node);
}

void BTGraphEditor::erase_node(BTGraphNode* bt_graph_node)
{
    ERR_FAIL_COND(bt_graph_node == nullptr);

    this->name_to_node.erase(bt_graph_node->get_name());
    this->task_to_node.erase(bt_graph_node->get_task());
}

void BTGraphEditor::delete_nodes(const godot::Vector<BTGraphNode*>& nodes_to_delete)
{
    int size = nodes_to_delete.size();
    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Delete node(s)");

    for (int i = 0; i < size; i++)
    {
        godot::Ref<BTTask> task_to_remove = nodes_to_delete[i]->get_task();
        godot::Array task_children = task_to_remove->get_children();
        godot::Ref<BTTask> parent = task_to_remove->get_parent();
        int task_id = this->behaviour_tree->get_task_id(task_to_remove);

        undo_redo_manager->add_do_method(this->behaviour_tree, "set_tasks_of_parent", task_to_remove, godot::Array());
        undo_redo_manager->add_do_method(this->behaviour_tree, "remove_task_by_ref", task_to_remove);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "add_task", task_id, task_to_remove);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "set_tasks_of_parent", task_to_remove, task_children);

        /* add node first when undoing so connections can form*/
        undo_redo_manager->add_undo_method(this->graph_edit, "add_child", nodes_to_delete[i]);
        undo_redo_manager->add_undo_method(this, "insert_node", nodes_to_delete[i]);

        for (int j = 0; j < task_children.size(); j++)
        {
            /* remove_task_by_ref will remove connections in bt_tasks*/
            undo_redo_manager->add_do_method(this->graph_edit, "disconnect_node", nodes_to_delete[i]->get_name(), 0, task_to_node[task_children[j]]->get_name(), 0);
            undo_redo_manager->add_undo_method(this->graph_edit, "connect_node", nodes_to_delete[i]->get_name(), 0, task_to_node[task_children[j]]->get_name(), 0);
        }

        if (parent.is_valid())
        {
            int index = parent->get_child_index(task_to_remove);
            undo_redo_manager->add_undo_method(this->behaviour_tree, "connect_tasks", parent, task_to_remove, index);
            undo_redo_manager->add_do_method(this->graph_edit, "disconnect_node", task_to_node[parent]->get_name(), 0, nodes_to_delete[i]->get_name(), 0);
            undo_redo_manager->add_undo_method(this->graph_edit, "connect_node", task_to_node[parent]->get_name(), 0, nodes_to_delete[i]->get_name(), 0);
        }

        if (this->behaviour_tree->get_root_task() == task_to_remove)
        {
            undo_redo_manager->add_undo_method(this->behaviour_tree, "set_root_task", task_to_remove);
            undo_redo_manager->add_undo_method(this, "evaluate_root_node");
        }
        /* remove node last safely */
        undo_redo_manager->add_do_method(this->graph_edit, "remove_child", nodes_to_delete[i]);
        undo_redo_manager->add_do_method(this, "erase_node", nodes_to_delete[i]);
        undo_redo_manager->add_do_method(this, "evaluate_root_node");

        undo_redo_manager->add_undo_method(this, "evaluate_root_node");
    }

    undo_redo_manager->commit_action();
}

void BTGraphEditor::clear_graph_nodes()
{
    this->graph_edit->clear_connections();

    godot::TypedArray<godot::Node> children = this->graph_edit->get_children();
    for (int i = 0, size = children.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(children[i]);

        if (bt_graph_node != nullptr)
        {
            this->erase_node(bt_graph_node);
            this->graph_edit->remove_child(bt_graph_node);
            bt_graph_node->queue_free();
        }
    }
}

void BTGraphEditor::create_default_graph_nodes()
{
    godot::Array tasks = this->behaviour_tree->get_tasks();
    this->task_to_node.clear();
    this->name_to_node.clear();

    for (int i = 0, size = tasks.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = new_bt_graph_node_from_task(godot::Ref<BTTask>(tasks[i]));
        bt_graph_node->set_title(bt_graph_node->get_task()->get_custom_name());

        int id = this->behaviour_tree->get_task_id(godot::Ref<BTTask>(tasks[i]));
        bt_graph_node->set_name(godot::itos(id));

        this->insert_node(bt_graph_node);

        this->graph_edit->add_child(bt_graph_node);

        bt_graph_node->call_deferred("connect", "dragged", callable_mp(this, &BTGraphEditor::_node_dragged).bind(bt_graph_node->get_name()));
        bt_graph_node->call_deferred("connect", "double_clicked", callable_mp(this, &BTGraphEditor::_on_node_double_clicked));
        bt_graph_node->call_deferred("connect", "right_clicked", callable_mp(this, &BTGraphEditor::_on_node_right_clicked));
    }

    for (int i = 0, size = tasks.size(); i < size; i++)
    {
        BTGraphNode* parent_node = this->task_to_node[godot::Ref<BTTask>(tasks[i])];
        godot::Array children = parent_node->get_task()->get_children();
        for (int j = 0, size = children.size(); j < size; j++)
        {
            BTGraphNode* child_node = this->task_to_node[godot::Ref<BTTask>(children[j])];
            this->graph_edit->connect_node(parent_node->get_name(), 0, child_node->get_name(), 0);
        }
    }
    this->evaluate_root_node();
    this->arrange_nodes();

}

void BTGraphEditor::set_root_node(BTGraphNode* new_root_node)
{
    BTGraphNode* old_root_node = task_to_node[this->behaviour_tree->get_root_task()];
    
    if (new_root_node == old_root_node)
    {
        return;
    }

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Set root node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "set_root_task", new_root_node->get_task());
    undo_redo_manager->add_do_method(this, "evaluate_root_node");

    undo_redo_manager->add_undo_method(this->behaviour_tree, "set_root_task", old_root_node->get_task());
    undo_redo_manager->add_undo_method(this, "evaluate_root_node");

    undo_redo_manager->commit_action();
}

void BTGraphEditor::arrange_nodes()
{
    if (!(this->behaviour_tree->get_root_task().is_valid()))
    {
        return;
    }

    BTGraphNode* root_node = this->task_to_node[this->behaviour_tree->get_root_task()];
    ERR_FAIL_COND(root_node == nullptr);

    godot::Vector<godot::Pair<BTGraphNode*, int>> stack;
    this->_extract_node_levels_into_stack(root_node, stack);

    godot::HashMap<int, int> level_to_node_count;
    godot::Vector2 root_node_position = root_node->get_position_offset();
    godot::Vector2 node_padding = root_node->get_size() + godot::Vector2(80, 20);
    int current_index = stack.size() - 1;

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Arrange node(s)");

    while (current_index >= 0)
    {
        godot::Pair<BTGraphNode*, int> pair_node_level = stack[current_index];

        godot::Vector2 position = godot::Vector2(0, 0);
        position.x += node_padding.x*pair_node_level.second;
        position.y += node_padding.y*level_to_node_count[pair_node_level.second];
        position += root_node_position;

        /*NOTE: won't save in scene of behaviour tree*/
        undo_redo_manager->add_do_method(pair_node_level.first, "set_position_offset", position);
        undo_redo_manager->add_undo_method(pair_node_level.first, "set_position_offset", pair_node_level.first->get_position_offset());

        level_to_node_count[pair_node_level.second]++;
        current_index--;
    }

    undo_redo_manager->commit_action();
}

void BTGraphEditor::evaluate_root_node()
{
    godot::Ref<BTTask> root = this->behaviour_tree->get_root_task();

    for (const godot::KeyValue<godot::Ref<BTTask>, BTGraphNode*>& element : task_to_node)
    {
        element.value->set_self_modulate(godot::Color::named("WHITE"));
    }

    if (root.is_valid())
    {
        BTGraphNode* root_node = task_to_node[this->behaviour_tree->get_root_task()];

        ERR_FAIL_COND_MSG(root_node == nullptr, "Invalid root node for coloring.");
        root_node->set_self_modulate(godot::Color::named("BLUE"));
    }
}

/* Drag and Drop */

void BTGraphEditor::_node_dragged(const godot::Vector2 &_from, const godot::Vector2 &_to, godot::StringName node_name)
{
    ERR_FAIL_COND_MSG(!(this->name_to_node.has(node_name)), "Dragged an invalid node");

    this->drag_buffer.push_back({_from, _to, node_name});
    if (!this->drag_called)
    {
        this->drag_called = true;
        this->call_deferred(godot::StringName("_move_nodes"));
    }
}

void BTGraphEditor::_move_nodes()
{
    this->drag_called = false;

    godot::EditorUndoRedoManager* undo_redo = this->editor_plugin->get_undo_redo();

    godot::HashSet<godot::Ref<BTTask>> sorted_parents;
        
    undo_redo->create_action("Move node(s)");

    for (DragOperation operation : this->drag_buffer)
    {
        godot::Ref<BTTask> parent = this->name_to_node[operation.node_name]->get_task()->get_parent();
        if (!parent.is_valid())
        {
            continue;
        }
        if (!(sorted_parents.has(parent)))
        {
            godot::Array old_children = parent->get_children();
            godot::Array new_children = this->get_sorted_by_y_children_of_parent(this->task_to_node[parent]);
            undo_redo->add_do_method(this->behaviour_tree, "set_tasks_of_parent", parent, new_children);
            undo_redo->add_undo_method(this->behaviour_tree, "set_tasks_of_parent", parent, old_children);
            sorted_parents.insert(parent);
        }
    }

    for (DragOperation& operation : drag_buffer)
    {
        BTGraphNode* dragged_graph_node = this->name_to_node[operation.node_name];

        undo_redo->add_do_method(dragged_graph_node, "set_position_offset", operation.to_position);
        undo_redo->add_undo_method(dragged_graph_node, "set_position_offset", operation.from_position);
    }

    undo_redo->commit_action();

    drag_buffer.clear();
}

/* Event Handlers */

void BTGraphEditor::_add_new_node_button_pressed()
{
    BTGraphNode* bt_graph_node = this->new_bt_graph_node();
    ERR_FAIL_COND(bt_graph_node == nullptr);


    int id = this->behaviour_tree->get_valid_id();
    bt_graph_node->set_title(godot::itos(id));
    bt_graph_node->set_name(godot::itos(id));
    bt_graph_node->get_task()->set_custom_name(godot::itos(id));

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Add a node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "add_task", id, bt_graph_node->get_task());
    undo_redo_manager->add_do_method(this->graph_edit, "add_child", bt_graph_node);
    undo_redo_manager->add_do_method(this, "insert_node", bt_graph_node);
    undo_redo_manager->add_do_method(this, "evaluate_root_node");

    undo_redo_manager->add_undo_method(this, "erase_node", bt_graph_node);
    undo_redo_manager->add_undo_method(this->graph_edit, "remove_child", bt_graph_node);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task", id);
    undo_redo_manager->add_undo_method(this, "evaluate_root_node");

    undo_redo_manager->commit_action();

    /* TODO:
    /* bt_graph_node->connect("node_selected", callable_mp(this, &BTEditorPlugin::_node_selected).bind(id));
    /* bt_graph_node->connect("node_deselected", callable_mp(this, &BTEditorPlugin::_node_deselected).bind(id)); */

    bt_graph_node->call_deferred("connect", "dragged", callable_mp(this, &BTGraphEditor::_node_dragged).bind(bt_graph_node->get_name()));
    bt_graph_node->call_deferred("connect", "double_clicked", callable_mp(this, &BTGraphEditor::_on_node_double_clicked));
    bt_graph_node->call_deferred("connect", "right_clicked", callable_mp(this, &BTGraphEditor::_on_node_right_clicked));

}

void BTGraphEditor::_arrange_nodes_button_pressed()
{
    this->arrange_nodes();
}

void BTGraphEditor::_on_rename_edit_text_submitted(const godot::String& new_text)
{
    ERR_FAIL_COND(this->last_double_clicked_node == nullptr);

    ERR_FAIL_COND_EDMSG(new_text == "", "Name of node must not be empty.");

    const godot::String& old_text = this->last_double_clicked_node->get_title();

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Rename node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "set_custom_name_task_by_ref", this->last_double_clicked_node->get_task(), new_text);
    undo_redo_manager->add_do_method(this->last_double_clicked_node, "set_title", new_text);

    undo_redo_manager->add_undo_method(this->last_double_clicked_node, "set_title", old_text);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "set_custom_name_task_by_ref", this->last_double_clicked_node->get_task(), old_text);

    undo_redo_manager->commit_action();

    this->rename_edit->set_visible(false);
}

void BTGraphEditor::_on_rename_edit_focus_exited()
{
    this->rename_edit->set_visible(false);
}

void BTGraphEditor::_on_node_double_clicked(BTGraphNode* clicked_node)
{
    ERR_FAIL_COND(clicked_node == nullptr);

    this->last_double_clicked_node = clicked_node;
    this->rename_edit->set_text(clicked_node->get_title());
    this->rename_edit->set_visible(true);
    /* grab_focus should be a deferred call. See:
     * https://docs.godotengine.org/en/stable/classes/class_control.html#class-control-method-grab-focus */
    this->rename_edit->call_deferred("grab_focus");
    this->rename_edit->set_position(godot::Vector2(clicked_node->get_position().x, 
                                                   clicked_node->get_position().y - this->rename_edit->get_size().y));
    
    /* show up on front not behind */
    this->rename_edit->set_z_index(clicked_node->get_z_index() + 1);
}

void BTGraphEditor::_on_node_right_clicked(BTGraphNode* clicked_node)
{
    ERR_FAIL_COND(clicked_node == nullptr);

    this->last_right_clicked_node = clicked_node;

    godot::Vector2 menu_position = clicked_node->get_global_position();
    int current_screen = clicked_node->get_viewport()->get_window()->get_current_screen();

    menu_position.y += clicked_node->get_size().y;

    this->main_popup_menu->set_visible(true);
    this->main_popup_menu->set_position(menu_position);
    this->main_popup_menu->set_current_screen(current_screen);
    this->main_popup_menu->call_deferred("grab_focus");

    godot::Vector<godot::StringName> task_names = 
    {
        BTTask::get_class_static(),
        BTSelector::get_class_static(),
        BTSequence::get_class_static(),
        BTRandomSelector::get_class_static(),
        BTRandomSequence::get_class_static(),
        BTAlwaysFail::get_class_static(),
        BTAlwaysSucceed::get_class_static(),
        BTInvert::get_class_static(),
        BTProbability::get_class_static(),
        BTRepeat::get_class_static(),
        BTAction::get_class_static(),
    };

    /* TODO: find better solution. */
    for (int i = 0, size = task_names.size(); i < size; i++)
    {
        this->task_type_popup_menu->set_item_checked(i, false);
    }

    godot::Ref<BTTask> task = clicked_node->get_task();
    int id = task_names.find(task->get_class());
    this->task_type_popup_menu->set_item_checked(id, true);
}

void BTGraphEditor::_on_main_popup_menu_item_selected(int id)
{
    godot::String item = this->main_popup_menu->get_item_text(id);
    if (item == "Change name")
    {
        /* TODO: fix this abomination*/
        this->_on_node_double_clicked(this->last_right_clicked_node);
        return;
    }
    else if (item == "Set root")
    {
        this->set_root_node(this->last_right_clicked_node);
        return;
    }
    else if (item == "Delete")
    {
        this->delete_nodes({this->last_right_clicked_node});
    }
}

void BTGraphEditor::_on_task_type_popup_menu_item_selected(int id)
{
    godot::String item = this->task_type_popup_menu->get_item_text(id);

    this->change_task_type(item, this->last_right_clicked_node);
}

void BTGraphEditor::_on_main_popup_menu_close_requested()
{
    this->graph_edit->call_deferred("grab_focus");
}

void BTGraphEditor::_delete_nodes_request(godot::TypedArray<godot::StringName> _nodes_to_delete)
{
    godot::Vector<BTGraphNode*> nodes_to_delete;
    int size = _nodes_to_delete.size();
    nodes_to_delete.resize(size);

    ERR_FAIL_COND(size == 0);

    /* Honestly, the best solution I can think of right now is to just disable it,
     * because undoing all the deleted connections will be a pain.
     * https://en.wikipedia.org/wiki/Ostrich_algorithm .*/
    ERR_FAIL_COND_EDMSG(size != 1, "Cannot delete mutliple nodes.");

    for (int i = 0; i < size; i++)
    {
        BTGraphNode* node_to_delete = this->name_to_node[_nodes_to_delete[i]];
        ERR_FAIL_COND(node_to_delete == nullptr);
        nodes_to_delete.set(i, node_to_delete);
    }

    this->delete_nodes(nodes_to_delete);
    
}


/* Connection Handling */


void BTGraphEditor::connection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port)
{
    ERR_FAIL_COND_MSG(!(this->name_to_node.has(_from_node)), "From_node doesn't exist.");
    ERR_FAIL_COND_MSG(!(this->name_to_node.has(_to_node)), "To_node doesn't exist.");

    BTGraphNode* from_node = this->name_to_node[_from_node];
    BTGraphNode* to_node = this->name_to_node[_to_node];

    bool can_connect = this->behaviour_tree->can_connect(from_node->get_task(), to_node->get_task());
    if (can_connect)
    {
        godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

        undo_redo_manager->create_action("Create a connection.");

        int index = this->get_node_insert_index_by_y_in_children(from_node, to_node);
        undo_redo_manager->add_do_method(this->behaviour_tree, "connect_tasks", from_node->get_task(), to_node->get_task(), index);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "disconnect_tasks", from_node->get_task(), to_node->get_task());

        undo_redo_manager->add_do_method(this->graph_edit, "connect_node", from_node->get_name(), from_port, to_node->get_name(), to_port);
        undo_redo_manager->add_undo_method(this->graph_edit, "disconnect_node", from_node->get_name(), from_port, to_node->get_name(), to_port);

        undo_redo_manager->commit_action();
    }
}

void BTGraphEditor::disconnection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port)
{
    ERR_FAIL_COND_MSG(!(this->name_to_node.has(_from_node)), "From_node doesn't exist.");
    ERR_FAIL_COND_MSG(!(this->name_to_node.has(_to_node)), "To_node doesn't exist.");

    BTGraphNode* from_node = this->name_to_node[_from_node];
    BTGraphNode* to_node = this->name_to_node[_to_node];

    bool can_disconnect = this->behaviour_tree->can_disconnect(from_node->get_task(), to_node->get_task());
    if (can_disconnect)
    {
        godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

        undo_redo_manager->create_action("Remove a connection.");

        int index = from_node->get_task()->get_child_index(to_node->get_task());
        undo_redo_manager->add_do_method(this->behaviour_tree, "disconnect_tasks", from_node->get_task(), to_node->get_task());
        undo_redo_manager->add_undo_method(this->behaviour_tree, "connect_tasks", from_node->get_task(), to_node->get_task(), index);

        
        undo_redo_manager->add_do_method(this->graph_edit, "disconnect_node", from_node->get_name(), from_port, to_node->get_name(), to_port);
        undo_redo_manager->add_undo_method(this->graph_edit, "connect_node", from_node->get_name(), from_port, to_node->get_name(), to_port);

        undo_redo_manager->commit_action();
    }
}

/* Task Management */

void BTGraphEditor::change_task_type(const godot::StringName& class_name, BTGraphNode* node)
{
    if (node->get_task()->get_class() == class_name)
    {
        return;
    }

    godot::Ref<BTTask> new_task = godot::ClassDB::instantiate(class_name);
    godot::Ref<BTTask> old_task = node->get_task();
    
    ERR_FAIL_COND(new_task == nullptr);
    ERR_FAIL_COND(old_task == nullptr);
    
    new_task->set_custom_name(node->get_task()->get_custom_name());
    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Change task type.");
    undo_redo_manager->add_do_method(this->behaviour_tree, "swap_task_in", old_task, new_task);
    /* reset node */
    undo_redo_manager->add_do_method(this, "erase_node", node);
    undo_redo_manager->add_do_method(node, "set_task", new_task);
    undo_redo_manager->add_do_method(this, "insert_node", node);

    undo_redo_manager->add_undo_method(this->behaviour_tree, "swap_task_in", new_task, old_task);
    /* reset node */
    undo_redo_manager->add_undo_method(this, "erase_node", node);
    undo_redo_manager->add_undo_method(node, "set_task", old_task);
    undo_redo_manager->add_undo_method(this, "insert_node", node);

    undo_redo_manager->commit_action();
}


/* Getters and Setters */


void BTGraphEditor::set_editor_plugin(godot::EditorPlugin* editor_plugin)
{
    ERR_FAIL_COND(editor_plugin == nullptr);
    this->editor_plugin = editor_plugin;
}

void BTGraphEditor::set_behaviour_tree(BehaviourTree* new_tree)
{
    ERR_FAIL_COND(new_tree == nullptr);
    this->behaviour_tree = new_tree;
    this->clear_graph_nodes();
    this->create_default_graph_nodes();
}

void BTGraphEditor::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("insert_node", "bt_graph_node"), &BTGraphEditor::insert_node);
    ClassDB::bind_method(D_METHOD("erase_node", "bt_graph_node"), &BTGraphEditor::erase_node);
    ClassDB::bind_method(D_METHOD("_move_nodes"), &BTGraphEditor::_move_nodes);
    ClassDB::bind_method(D_METHOD("evaluate_root_node"), &BTGraphEditor::evaluate_root_node);
    ClassDB::bind_method(D_METHOD("set_editor_plugin", "editor_plugin"), &BTGraphEditor::set_editor_plugin);
    ClassDB::bind_method(D_METHOD("get_graph_edit"), &BTGraphEditor::get_graph_edit);
    ClassDB::bind_method(D_METHOD("new_bt_graph_node"), &BTGraphEditor::new_bt_graph_node);
    ClassDB::bind_method(D_METHOD("new_bt_graph_node_from_task", "bt_task"), &BTGraphEditor::new_bt_graph_node_from_task);
    ClassDB::bind_method(D_METHOD("get_graph_nodes"), &BTGraphEditor::get_graph_nodes);
    ClassDB::bind_method(D_METHOD("get_sorted_by_y_children_of_parent", "parent_graph_node"), &BTGraphEditor::get_sorted_by_y_children_of_parent);
    ClassDB::bind_method(D_METHOD("get_node_insert_index_by_y_in_children", "parent_graph_node", "graph_node"), &BTGraphEditor::get_node_insert_index_by_y_in_children);
    ClassDB::bind_method(D_METHOD("_add_new_node_button_pressed"), &BTGraphEditor::_add_new_node_button_pressed);
    ClassDB::bind_method(D_METHOD("_arrange_nodes_button_pressed"), &BTGraphEditor::_arrange_nodes_button_pressed);
    ClassDB::bind_method(D_METHOD("_node_dragged", "from", "to", "node_name"), &BTGraphEditor::_node_dragged);
    ClassDB::bind_method(D_METHOD("connection_request", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::connection_request);
    ClassDB::bind_method(D_METHOD("disconnection_request", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::disconnection_request);
    ClassDB::bind_method(D_METHOD("_delete_nodes_request", "nodes_to_delete"), &BTGraphEditor::_delete_nodes_request);
    ClassDB::bind_method(D_METHOD("clear_graph_nodes"), &BTGraphEditor::clear_graph_nodes);
    ClassDB::bind_method(D_METHOD("create_default_graph_nodes"), &BTGraphEditor::create_default_graph_nodes);
    ClassDB::bind_method(D_METHOD("arrange_nodes"), &BTGraphEditor::arrange_nodes);

     /* TODO: Can't pass godot::Vectpr<>& as variant. Check*/
    /*ClassDB::bind_method(D_METHOD("_extract_node_levels_into_stack", "root_node", "stack", "current_level"), &BTGraphEditor::_extract_node_levels_into_stack);*/

    ClassDB::bind_method(D_METHOD("set_behaviour_tree", "behaviour_tree"), &BTGraphEditor::set_behaviour_tree);
    ClassDB::bind_method(D_METHOD("get_behaviour_tree"), &BTGraphEditor::get_behaviour_tree);

}