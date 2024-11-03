#include "bt_graph_editor.hpp"
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_interface.hpp>

#include "behaviour_tree/utils/utils.hpp"

BTGraphEditor::BTGraphEditor()
{
    this->_setup_graph_edit();
    this->_setup_task_names();
    this->_setup_rename_edit();
    this->_setup_path_edit();
    this->_setup_popup_menu();

    this->drag_called = false;

    task_to_node[nullptr] = nullptr;

    this->graph_view = memnew(BTGraphView);
}

void BTGraphEditor::delete_saved_trees()
{
    for (godot::KeyValue<BehaviourTree*, godot::Vector<BTGraphNode*>> key_value : this->saved_trees)
    {
        for (BTGraphNode* node : key_value.value)
        {
            ERR_CONTINUE(node == nullptr);
            memdelete(node);
        }
    }
}

BTGraphEditor::~BTGraphEditor()
{
    if (behaviour_tree != nullptr)
    {
        /* Save for later deletion*/
        this->save_tree();
        this->clear_graph_nodes();
    }
    this->clear_copied_nodes();
    this->delete_saved_trees();
    memdelete(this->graph_edit);
    memdelete(this->graph_view);
}

/* Setup Methods */
void BTGraphEditor::_setup_graph_edit()
{
    this->graph_edit = memnew(godot::GraphEdit);
    this->graph_edit->set_h_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);
    this->graph_edit->set_v_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);

    this->graph_edit->call_deferred("connect", "connection_request", callable_mp(this, &BTGraphEditor::connection_request));
    this->graph_edit->call_deferred("connect", "disconnection_request", callable_mp(this, &BTGraphEditor::disconnection_request));
    this->graph_edit->call_deferred("connect", "delete_nodes_request", callable_mp(this, &BTGraphEditor::_delete_nodes_request));
    this->graph_edit->call_deferred("connect", "copy_nodes_request", callable_mp(this, &BTGraphEditor::copy_nodes_request));
    this->graph_edit->call_deferred("connect", "paste_nodes_request", callable_mp(this, &BTGraphEditor::paste_nodes_request));

    /* right to left */
    this->graph_edit->add_valid_connection_type(1, 0);
    this->graph_edit->add_valid_left_disconnect_type(0);

    this->graph_edit->set_right_disconnects(true);

    this->graph_edit->set_show_arrange_button(false);
}

void BTGraphEditor::_setup_task_names()
{
    this->composite_names = utils::get_subclasses(BTComposite::get_class_static());
    this->decorator_names = utils::get_subclasses(BTDecorator::get_class_static());
}

void BTGraphEditor::_setup_rename_edit()
{
    ERR_FAIL_COND(this->graph_edit == nullptr);

    this->rename_edit = memnew(godot::LineEdit);
    this->rename_edit->set_visible(false);
    this->graph_edit->add_child(this->rename_edit);

    this->rename_edit->call_deferred("connect", "text_submitted", callable_mp(this, &BTGraphEditor::_on_rename_edit_text_submitted));
    this->rename_edit->call_deferred("connect", "focus_exited", callable_mp(this, &BTGraphEditor::_on_rename_edit_focus_exited));
}

void BTGraphEditor::_setup_path_edit()
{
    ERR_FAIL_COND(this->graph_edit == nullptr);

    this->path_edit = memnew(godot::LineEdit);
    this->path_edit->set_visible(false);
    this->graph_edit->add_child(this->path_edit);

    this->path_edit->call_deferred("connect", "text_submitted", callable_mp(this, &BTGraphEditor::_on_path_edit_text_submitted));
    this->path_edit->call_deferred("connect", "focus_exited", callable_mp(this, &BTGraphEditor::_on_path_edit_focus_exited));
}

void BTGraphEditor::_setup_popup_menu()
{
    ERR_FAIL_COND(this->graph_edit == nullptr);

    this->main_popup_menu = memnew(godot::PopupMenu);
    this->main_popup_menu->set_name("MainPopupMenu");

    this->task_type_popup_menu = memnew(godot::PopupMenu);
    this->task_type_popup_menu->set_name("TaskTypePopupMenu");

    this->task_type_popup_menu->add_separator("Composites");

    for (const godot::StringName& task_name : this->composite_names)
    {
        this->task_type_popup_menu->add_radio_check_item(task_name);
    }

    this->task_type_popup_menu->add_separator("Decorators");
    for (const godot::StringName& task_name : this->decorator_names)
    {
        this->task_type_popup_menu->add_radio_check_item(task_name);
    }

    this->task_type_popup_menu->add_separator("Actions");
    this->task_type_popup_menu->add_item(BTAction::get_class_static());
    this->task_type_popup_menu->add_item(BTCondition::get_class_static());

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

    this->action_condition_type_popup_menu = memnew(godot::PopupMenu);
    this->action_condition_type_popup_menu->set_allow_search(true);
    this->graph_edit->add_child(this->action_condition_type_popup_menu);

    this->action_condition_type_popup_menu->call_deferred("connect", "id_pressed", callable_mp(this, &BTGraphEditor::_on_action_condition_type_popup_menu_item_selected));
}

void BTGraphEditor::_fill_action_condition_type_popup_menu(const godot::StringName& action_condition)
{
    ERR_FAIL_COND(this->action_condition_type_popup_menu == nullptr);
    this->action_condition_type_popup_menu->clear(true);
    godot::Vector<godot::StringName> subclasses = utils::get_subclasses(action_condition);
    for (int i = 0; i < subclasses.size(); i++)
    {
        this->action_condition_type_popup_menu->add_item(subclasses[i]);
    }
}

/* Utility Methods */

void BTGraphEditor::connect_graph_node_signals(BTGraphNode *node)
{
    ERR_FAIL_COND(node == nullptr);
    node->set_focus_mode(godot::Control::FocusMode::FOCUS_CLICK);
    node->call_deferred("connect", "dragged", callable_mp(this, &BTGraphEditor::_node_dragged).bind(node));
    node->call_deferred("connect", "node_selected", callable_mp(this, &BTGraphEditor::_on_node_selected).bind(node));
    node->call_deferred("connect", "node_deselected", callable_mp(this, &BTGraphEditor::_on_node_deselected).bind(node));

    if (godot::Object::cast_to<BTGraphNodeSubtree>(node) != nullptr)
    {
        node->call_deferred("connect", "double_clicked", callable_mp(this, &BTGraphEditor::_on_node_subtree_double_clicked).bind(node));
        node->call_deferred("connect", "right_clicked", callable_mp(this, &BTGraphEditor::_on_node_subtree_right_clicked).bind(node));
    }
    else
    {
        node->call_deferred("connect", "double_clicked", callable_mp(this, &BTGraphEditor::_on_node_double_clicked).bind(node));
        node->call_deferred("connect", "right_clicked", callable_mp(this, &BTGraphEditor::_on_node_right_clicked).bind(node));
    }
}

BTGraphNode* BTGraphEditor::new_graph_node()
{
    godot::Ref<BTTask> bt_task = godot::Ref<BTTask>(memnew(BTTask));
    BTGraphNode* bt_graph_node = this->new_bt_graph_node_from_task(bt_task);

    return bt_graph_node;
}

BTGraphNodeSubtree* BTGraphEditor::new_graph_node_subtree()
{
    godot::Ref<BTSubtree> bt_subtree = godot::Ref<BTSubtree>(memnew(BTSubtree));
    BTGraphNodeSubtree* bt_graph_node = this->new_bt_graph_node_subtree_from_task(bt_subtree);

    return bt_graph_node;
}

BTGraphNode* BTGraphEditor::new_bt_graph_node_from_task(godot::Ref<BTTask> bt_task)
{
    BTGraphNode* bt_graph_node = memnew(BTGraphNode);
    bt_graph_node->set_task(bt_task);

    return bt_graph_node;
}

BTGraphNodeSubtree* BTGraphEditor::new_bt_graph_node_subtree_from_task(godot::Ref<BTSubtree> bt_subtree)
{
    BTGraphNodeSubtree* bt_graph_node_subtree = memnew(BTGraphNodeSubtree);
    bt_graph_node_subtree->set_task(bt_subtree);
    
    return bt_graph_node_subtree;
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

void BTGraphEditor::name_node(BTGraphNode* node)
{
    ERR_FAIL_NULL(node);
    ERR_FAIL_NULL(node->get_task());
    int id = this->behaviour_tree->get_task_id(node->get_task());
    node->set_title(node->get_task()->get_custom_name());
    node->set_name(godot::itos(id));
}

void BTGraphEditor::save_tree()
{
    ERR_FAIL_NULL(this->behaviour_tree);

    godot::Vector<BTGraphNode*> graph_nodes;
    for (godot::KeyValue<godot::Ref<BTTask>, BTGraphNode*> element : this->task_to_node)
    {
        ERR_CONTINUE(element.key == nullptr);
        ERR_CONTINUE(element.value == nullptr);
        graph_nodes.push_back(element.value);
    }
    this->saved_trees.insert(this->behaviour_tree, graph_nodes);
}

void BTGraphEditor::load_tree()
{
    ERR_FAIL_COND(!(this->task_to_node.is_empty()));
    ERR_FAIL_COND(!(this->name_to_node.is_empty()));
    ERR_FAIL_NULL(this->behaviour_tree);
    godot::Vector<BTGraphNode*> saved_nodes = this->saved_trees[this->behaviour_tree];
    for (BTGraphNode* saved_node : saved_nodes)
    {
        ERR_CONTINUE(saved_node == nullptr);
        this->insert_node(saved_node);
        this->graph_edit->add_child(saved_node);
        /* Should be already connected */
        // this->connect_graph_node_signals(bt_graph_node);
    }
    
    for (BTGraphNode* saved_node : saved_nodes)
    {
        ERR_CONTINUE(saved_node == nullptr);
        godot::Array children = saved_node->get_task()->get_children();
        for (int i = 0, size = children.size(); i < size; i++)
        {
            ERR_CONTINUE(!(this->task_to_node.has(godot::Ref<BTTask>(children[i]))));
            BTGraphNode* child_node = this->task_to_node[godot::Ref<BTTask>(children[i])];
            this->graph_edit->connect_node(saved_node->get_name(), 0, child_node->get_name(), 0);
        }
    }

    this->color_root_node();
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

        undo_redo_manager->add_do_method(this->behaviour_tree, "set_children_of_task", task_to_remove, godot::Array());
        undo_redo_manager->add_do_method(this->behaviour_tree, "remove_task_by_ref", task_to_remove);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "add_task", task_id, task_to_remove);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "set_children_of_task", task_to_remove, task_children);

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
            undo_redo_manager->add_undo_method(this, "color_root_node");
        }
        /* remove node last safely */
        undo_redo_manager->add_do_method(this->graph_edit, "remove_child", nodes_to_delete[i]);
        undo_redo_manager->add_do_method(this, "erase_node", nodes_to_delete[i]);
        undo_redo_manager->add_do_method(this, "color_root_node");

        undo_redo_manager->add_undo_method(this, "color_root_node");
    }

    undo_redo_manager->commit_action();
}

void BTGraphEditor::clear_graph_nodes()
{
    if (this->behaviour_tree == nullptr)
    {
        return;
    }
    this->graph_edit->clear_connections();
    this->task_to_node.clear();
    this->name_to_node.clear();

    godot::TypedArray<godot::Node> children = this->graph_edit->get_children();
    for (int i = 0, size = children.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(children[i]);

        if (bt_graph_node != nullptr)
        {
            this->graph_edit->remove_child(bt_graph_node);
        }
    }
}

void BTGraphEditor::create_default_graph_nodes()
{
    godot::Array tasks = this->behaviour_tree->get_tasks();
    ERR_FAIL_COND(!(this->task_to_node.is_empty()));
    ERR_FAIL_COND(!(this->name_to_node.is_empty()));

    for (int i = 0, size = tasks.size(); i < size; i++)
    {   
        BTGraphNode* bt_graph_node;
        if (godot::Ref<BTSubtree>(tasks[i]) != nullptr)
        {
            BTGraphNodeSubtree* bt_graph_node_subtree = new_bt_graph_node_subtree_from_task(godot::Ref<BTSubtree>(tasks[i]));
            bt_graph_node = bt_graph_node_subtree;
            
            bt_graph_node_subtree->set_file_path(godot::Ref<BTSubtree>(tasks[i])->get_file_path());
        }
        else
        {
            bt_graph_node = new_bt_graph_node_from_task(godot::Ref<BTTask>(tasks[i]));
            bt_graph_node->set_title(bt_graph_node->get_task()->get_custom_name());
        }

        int id = this->behaviour_tree->get_task_id(godot::Ref<BTTask>(tasks[i]));
        bt_graph_node->set_name(godot::itos(id));

        this->insert_node(bt_graph_node);

        this->graph_edit->add_child(bt_graph_node);
        this->connect_graph_node_signals(bt_graph_node);
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
    this->color_root_node();
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
    undo_redo_manager->add_do_method(this, "color_root_node");

    undo_redo_manager->add_undo_method(this->behaviour_tree, "set_root_task", old_root_node->get_task());
    undo_redo_manager->add_undo_method(this, "color_root_node");

    undo_redo_manager->commit_action();
}

void BTGraphEditor::arrange_nodes(bool with_undo_redo)
{
    if (!(this->behaviour_tree->get_root_task().is_valid()))
    {
        return;
    }

    godot::Ref<BTTask> root_task = this->behaviour_tree->get_root_task();
    godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_child_names;
    godot::Vector<godot::Ref<BTTask>> task_queue = {root_task};
    int task_queue_i = 0;
    while (task_queue_i < task_queue.size())
    {
        const godot::Ref<BTTask> task = task_queue[task_queue_i];
        if (task->get_child_count() <= 0)
        {
            task_queue_i++;
            continue;
        }
        parent_to_child_names.insert(task->get_name(), {});
        godot::Array children = task->get_children();
        for (int j = 0; j < children.size(); j++)
        {
            godot::Ref<BTTask> child = children[j];
            parent_to_child_names[task->get_name()].push_back(child->get_name());
            task_queue.push_back(child);
        }
    }

    if (with_undo_redo)
    {
        godot::HashMap<BTGraphNode*, godot::Vector2> node_positions = this->graph_view->get_arranged_nodes_position(root_task->get_name(), parent_to_child_names);
        godot::EditorUndoRedoManager* undo_redo = this->editor_plugin->get_undo_redo();
        undo_redo->create_action("Arrange nodes.");
        for (const godot::KeyValue<BTGraphNode*, godot::Vector2>& key_value : node_positions)
        {
            undo_redo->add_do_method(key_value.key, "set_position_offset", key_value.value);
            undo_redo->add_undo_method(key_value.key, "set_position_offset", key_value.key->get_position_offset());
        }
        undo_redo->commit_action();
    }
    else
    {
        this->graph_view->arrange_nodes(root_task->get_name(), parent_to_child_names);
    }
}

void BTGraphEditor::color_root_node()
{
    godot::Ref<BTTask> root = this->behaviour_tree->get_root_task();

    for (const godot::KeyValue<godot::Ref<BTTask>, BTGraphNode*>& element : task_to_node)
    {
        //TODO: clean up
        if (godot::Object::cast_to<BTGraphNodeSubtree>(element.value) != nullptr)
        {
            element.value->set_self_modulate(godot::Color::named("YELLOW"));
        }
        else 
        {
            element.value->set_self_modulate(godot::Color::named("WHITE"));
        }
    }

    if (root.is_valid())
    {
        BTGraphNode* root_node = task_to_node[root];

        ERR_FAIL_COND_MSG(root_node == nullptr, "Invalid root node for coloring.");
        root_node->set_self_modulate(godot::Color::named("BLUE"));
    }
}

void BTGraphEditor::deselect_all_nodes()
{
    for (godot::KeyValue<godot::Ref<BTTask>, BTGraphNode*> element : task_to_node)
    {
        ERR_CONTINUE(element.value == nullptr);
        ERR_CONTINUE(element.value->get_task().is_null());
        if (element.value->is_selected())
        {
            element.value->set_selected(false);
        }
    }
}

/* Drag and Drop */

void BTGraphEditor::_node_dragged(const godot::Vector2 &_from, const godot::Vector2 &_to, BTGraphNode *node)
{
    ERR_FAIL_COND(node == nullptr);

    this->drag_buffer.push_back({_from, _to, node});
    if (!this->drag_called)
    {
        this->drag_called = true;
        this->call_deferred(godot::StringName("_move_nodes"));
    }
}

void BTGraphEditor::_move_nodes()
{
    if (!this->drag_called)
    {
        return;
    }

    this->drag_called = false;

    godot::EditorUndoRedoManager* undo_redo = this->editor_plugin->get_undo_redo();

    godot::HashSet<godot::Ref<BTTask>> sorted_parents;
        
    undo_redo->create_action("Move node(s)");

    for (DragOperation operation : this->drag_buffer)
    {
        godot::StringName task_name = this->graph_view->get_task_name(operation.node->get_name());
        godot::Ref<BTTask> parent = this->behaviour_tree->get_task_by_name(task_name)->get_parent();
        if (!parent.is_valid())
        {
            continue;
        }
        if (!(sorted_parents.has(parent)))
        {
            godot::Array old_children = parent->get_children();
            godot::Vector<godot::StringName> children_names = BTGraphEditor::bttask_array_to_names(old_children);
            godot::Vector<godot::StringName> sorted_children = this->graph_view->sorted_task_names_by_y(children_names);
            godot::Array new_children;
            for (const godot::StringName& name : sorted_children)
            {
                new_children.push_back(this->behaviour_tree->get_task_by_name(name));
            }
            undo_redo->add_do_method(this->behaviour_tree, "set_children_of_task", parent, new_children);
            undo_redo->add_undo_method(this->behaviour_tree, "set_children_of_task", parent, old_children);
            sorted_parents.insert(parent);
        }
    }

    for (DragOperation& operation : drag_buffer)
    {
        undo_redo->add_do_method(operation.node, "set_position_offset", operation.to_position);
        undo_redo->add_undo_method(operation.node, "set_position_offset", operation.from_position);
    }

    undo_redo->commit_action();

    drag_buffer.clear();
}

/* Event Handlers */

void BTGraphEditor::_add_new_node_button_pressed()
{
    int id = this->behaviour_tree->get_valid_id();
    godot::Ref<BTTask> new_task = memnew(BTTask);
    godot::StringName task_custom_name = godot::itos(id);
    godot::StringName task_name = new_task->get_name();
    
    new_task->set_custom_name(task_custom_name);

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Add a node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "add_task", id, new_task);
    undo_redo_manager->add_do_method(this->graph_view, "create_task_node", task_name);
    undo_redo_manager->add_do_method(this->graph_view, "set_task_node_title", task_name, task_custom_name);
    //undo_redo_manager->add_do_method(this, "color_root_node");

    undo_redo_manager->add_undo_method(this->graph_view, "delete_task_node", task_name);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task", id);
    //undo_redo_manager->add_undo_method(this, "color_root_node");

    undo_redo_manager->commit_action();

    //this->connect_graph_node_signals(bt_graph_node);
}

void BTGraphEditor::_add_new_subtree_node_button_pressed()
{
    int id = this->behaviour_tree->get_valid_id();

    godot::Ref<BTSubtree> new_task = memnew(BTSubtree);
    godot::StringName task_custom_name = godot::itos(id);
    godot::StringName task_name = new_task->get_name();

    new_task->set_custom_name(task_custom_name);

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Add a subtree node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "add_task", id, new_task);
    undo_redo_manager->add_do_method(this->graph_view, "create_subtree_task_node", task_name);
    //undo_redo_manager->add_do_method(this, "color_root_node");

    undo_redo_manager->add_undo_method(this->graph_view, "delete_task_node", task_name);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task", id);
    //undo_redo_manager->add_undo_method(this, "color_root_node");

    undo_redo_manager->commit_action();

    //this->connect_graph_node_signals(bt_graph_node);
}

void BTGraphEditor::_arrange_nodes_button_pressed()
{
    this->arrange_nodes(true);
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

void BTGraphEditor::_on_path_edit_text_submitted(const godot::String& new_path)
{
    ERR_FAIL_COND(this->last_double_clicked_node == nullptr);
    ERR_FAIL_COND(godot::Object::cast_to<BTGraphNodeSubtree>(this->last_double_clicked_node) == nullptr);

    ERR_FAIL_COND_EDMSG(new_path == "", "Path must not be empty.");

    godot::Ref<BTSubtree> subtree_task = godot::Object::cast_to<BTGraphNodeSubtree>(this->last_double_clicked_node)->get_task();
    const godot::String& old_path = subtree_task->get_file_path();

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Change path of subtree.");

    /* register changes on scene. TODO: fix */
    undo_redo_manager->add_do_method(this->behaviour_tree, "set_root_task", this->behaviour_tree->get_root_task());
    undo_redo_manager->add_do_method(subtree_task.ptr(), "set_file_path", new_path);
    undo_redo_manager->add_do_method(this->last_double_clicked_node, "set_file_path", new_path);
    
    undo_redo_manager->add_undo_method(subtree_task.ptr(), "set_file_path", old_path);
    undo_redo_manager->add_undo_method(this->last_double_clicked_node, "set_file_path", old_path);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "set_root_task", this->behaviour_tree->get_root_task());

    undo_redo_manager->commit_action();

    this->rename_edit->set_visible(false);
}

void BTGraphEditor::_on_path_edit_focus_exited()
{
    this->path_edit->set_visible(false);
}

void BTGraphEditor::_on_node_selected(BTGraphNode* selected_node)
{
    ERR_FAIL_COND(selected_node == nullptr);
    godot::StringName task_name = this->graph_view->get_task_name(selected_node->get_name());
    godot::Ref<BTTask> task = this->behaviour_tree->get_task_by_name(task_name);

    godot::EditorInterface* editor_interface = godot::EditorInterface::get_singleton();
    editor_interface->inspect_object(task.ptr());
}

void BTGraphEditor::_on_node_deselected(BTGraphNode* deselected_node)
{
    ERR_FAIL_COND(deselected_node == nullptr);
    ERR_FAIL_COND(this->behaviour_tree == nullptr);

    godot::EditorInterface* editor_interface = godot::EditorInterface::get_singleton();
    editor_interface->inspect_object(this->behaviour_tree);
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
    this->rename_edit->set_size(godot::Size2(clicked_node->get_size().x, this->path_edit->get_size().y));
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

    clicked_node->focus_task_in_inspector();
    

    /* TODO: find better solution. */
    int size = this->composite_names.size() + this->decorator_names.size();
    for (int i = 0; i < size; i++)
    {
        this->task_type_popup_menu->set_item_checked(i, false);
    }

    godot::Ref<BTTask> task = clicked_node->get_task();
    int id1 = this->composite_names.find(task->get_class());
    int id2 = this->decorator_names.find(task->get_class());
    if (id1 != -1)
    {
        this->task_type_popup_menu->set_item_checked(id1, true);
    }
    else if (id2 != -1)
    {
        id2 += this->composite_names.size();
        this->task_type_popup_menu->set_item_checked(id2, true);
    }
}

void BTGraphEditor::_on_node_subtree_double_clicked(BTGraphNodeSubtree* clicked_node)
{
    ERR_FAIL_COND(clicked_node == nullptr);

    this->last_double_clicked_node = clicked_node;
    this->path_edit->set_text(clicked_node->get_file_path());
    this->path_edit->set_visible(true);
    this->path_edit->call_deferred("grab_focus");
    this->path_edit->set_size(godot::Size2(clicked_node->get_size().x, this->path_edit->get_size().y));
    this->path_edit->set_position(godot::Vector2(clicked_node->get_position().x, 
                                                   clicked_node->get_position().y - this->path_edit->get_size().y));
    
    this->path_edit->set_z_index(clicked_node->get_z_index() + 1);
}

void BTGraphEditor::_on_node_subtree_right_clicked(BTGraphNodeSubtree* clicked_node)
{

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
    godot::StringName item_text = this->task_type_popup_menu->get_item_text(id);

    if (item_text == BTAction::get_class_static() || item_text == BTCondition::get_class_static())
    {
        this->_on_action_condition_type_popup_menu_show(item_text);
    }
    else
    {
        this->change_task_type(item_text, this->last_right_clicked_node);
    }
}


void BTGraphEditor::_on_action_condition_type_popup_menu_show(const godot::StringName& action_condition)
{
    ERR_FAIL_COND(this->action_condition_type_popup_menu == nullptr);

    /* Refill every time because user might define new subclasses*/
    this->_fill_action_condition_type_popup_menu(action_condition);

    godot::Vector2 menu_position = last_right_clicked_node->get_global_position();
    int current_screen = last_right_clicked_node->get_viewport()->get_window()->get_current_screen();

    menu_position.y += last_right_clicked_node->get_size().y;

    this->action_condition_type_popup_menu->set_visible(true);
    this->action_condition_type_popup_menu->set_position(menu_position);
    this->action_condition_type_popup_menu->set_current_screen(current_screen);
    this->action_condition_type_popup_menu->call_deferred("grab_focus");
}

void BTGraphEditor::_on_action_condition_type_popup_menu_item_selected(int id)
{
    godot::StringName item = this->action_condition_type_popup_menu->get_item_text(id);
    this->change_task_type(item, this->last_right_clicked_node);
}

void BTGraphEditor::_delete_nodes_request(godot::TypedArray<godot::StringName> _nodes_to_delete)
{
    godot::Vector<BTGraphNode*> nodes_to_delete;
    int size = _nodes_to_delete.size();
    nodes_to_delete.resize(size);

    if (size == 0)
    {
        return;
    }

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


/* Copy-pasta Handling */

BTGraphNode* duplicate_graph_node(const BTGraphNode* node)
{
    /* Graph node is only a visual wrapper for task so no need to duplicate it. */
    BTGraphNode* copy_node = memnew(BTGraphNode);
    godot::Ref<BTTask> copy_task = node->get_task()->duplicate();

    copy_task->clear_children();

    copy_task->set_parent(nullptr);
    copy_node->set_task(copy_task);
    copy_node->set_position_offset(node->get_position_offset());
    return copy_node;
}


void BTGraphEditor::copy_nodes_request()
{
    /* Copied/selected nodes might be deleted later by user. So create copies now while still valid. */

    godot::HashSet<BTGraphNode*> selected_nodes;
    for (godot::KeyValue<godot::Ref<BTTask>, BTGraphNode*> element : task_to_node)
    {
        ERR_FAIL_COND(element.value == nullptr);
        ERR_FAIL_COND(element.value->get_task().is_null());
        if (element.value->is_selected())
        {
            selected_nodes.insert(element.value);
        }
    }

    godot::HashMap<BTGraphNode*, BTGraphNode*> old_to_new;

    for (BTGraphNode* node : selected_nodes)
    {
        BTGraphNode* copy_node = duplicate_graph_node(node);
        ERR_FAIL_NULL(copy_node);
        old_to_new.insert(node, copy_node);
    }

    this->clear_copied_nodes();

    for (BTGraphNode* node : selected_nodes)
    {
        godot::Array children = node->get_task()->get_children();
        for (int i = 0, size = children.size(); i < size; i++)
        {
            ERR_FAIL_COND(!(task_to_node.has(godot::Ref<BTTask>(children[i]))));
            BTGraphNode* child_node = task_to_node[godot::Ref<BTTask>(children[i])];
            if (selected_nodes.has(child_node))
            {
                /* Put new connections to be copied */
                this->copied_connections.push_back({old_to_new[node], old_to_new[child_node]});
            }
        }
    }

    for (BTGraphNode* node : selected_nodes)
    {
        this->copied_nodes.push_back(old_to_new[node]);
    }
}

void BTGraphEditor::paste_nodes_request()
{
    if (this->copied_nodes.size() == 0)
    {
        return;
    }

    godot::HashSet<BTGraphNode*> pasted_nodes;
    godot::HashMap<BTGraphNode*, BTGraphNode*> copied_to_pasted;
    for (BTGraphNode* copied_node : this->copied_nodes)
    {
        ERR_FAIL_COND(copied_node == nullptr);
        ERR_FAIL_COND(copied_node->get_task().is_null());
        
        BTGraphNode* pasted_node = duplicate_graph_node(copied_node);
        ERR_FAIL_NULL(pasted_node);
        pasted_nodes.insert(pasted_node);
        copied_to_pasted.insert(copied_node, pasted_node);
        
        // TODO: make signals disconnect on removal. 
        this->connect_graph_node_signals(pasted_node);
    }

    EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Paste copied nodes");

    for (BTGraphNode* pasted_node : pasted_nodes)
    {
        // TODO: Extract logic for adding new node to the graph.
        undo_redo_manager->add_do_method(this->behaviour_tree, "add_task_by_ref", pasted_node->get_task());
        undo_redo_manager->add_do_method(this->graph_edit, "add_child", pasted_node);
        undo_redo_manager->add_do_method(this, "name_node", pasted_node);
        undo_redo_manager->add_do_method(this, "insert_node", pasted_node);

        pasted_node->set_position_offset(pasted_node->get_position_offset() + Vector2(100, 100));
    }

    undo_redo_manager->add_do_method(this, "deselect_all_nodes");
    for (BTGraphNode* pasted_node : pasted_nodes)
    {
        /* Select pasted nodes for easy interaction */
        undo_redo_manager->add_do_method(pasted_node, "set_selected", true);
    }

    for (godot::Pair<BTGraphNode*, BTGraphNode*> copied_connection : this->copied_connections)
    {
        BTGraphNode* pasted_parent_node = copied_to_pasted[copied_connection.first];
        BTGraphNode* pasted_child_node = copied_to_pasted[copied_connection.second];

        undo_redo_manager->add_do_method(this, "_connect_nodes", pasted_parent_node, pasted_child_node);
        undo_redo_manager->add_undo_method(this, "_disconnect_nodes", pasted_parent_node, pasted_child_node);
    }

    /* Add undo node from graph */
    for (BTGraphNode* pasted_node : pasted_nodes)
    {
        undo_redo_manager->add_undo_method(this, "erase_node", pasted_node);
        undo_redo_manager->add_undo_method(this->graph_edit, "remove_child", pasted_node);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task_by_ref", pasted_node->get_task());
    }

    undo_redo_manager->add_do_method(this, "color_root_node");
    
    undo_redo_manager->commit_action();
}

void BTGraphEditor::clear_copied_nodes()
{
    this->copied_connections.clear();
    /* Clear old copied nodes */
    for (BTGraphNode* old_copy : this->copied_nodes)
    {
        ERR_FAIL_NULL(old_copy);
        memdelete(old_copy);
    }
    this->copied_nodes.clear();
}


/* Connection Handling */

godot::Vector<StringName> BTGraphEditor::bttask_array_to_names(godot::Array tasks)
{
    godot::Vector<StringName> result;
    for (int i = 0; i < tasks.size(); i++)
    {
        godot::Ref<BTTask> task = tasks[i];
        ERR_CONTINUE(task.is_null());
        result.push_back(godot::Ref<BTTask>(tasks[i])->get_name());
    }
    return result;
}

void BTGraphEditor::_connect_nodes(BTGraphNode* parent, BTGraphNode* child)
{
    ERR_FAIL_NULL(this->behaviour_tree);
    ERR_FAIL_NULL(child);
    ERR_FAIL_NULL(parent);
    ERR_FAIL_NULL(child->get_task());
    ERR_FAIL_NULL(parent->get_task());

    int index = this->get_node_insert_index_by_y_in_children(parent, child);
    this->behaviour_tree->connect_tasks(parent->get_task(), child->get_task(), index);
    // TODO: fix port numbers?
    this->graph_edit->connect_node(parent->get_name(), 0, child->get_name(), 0);
}

void BTGraphEditor::_disconnect_nodes(BTGraphNode* parent, BTGraphNode* child)
{
    ERR_FAIL_NULL(this->behaviour_tree);
    ERR_FAIL_NULL(child);
    ERR_FAIL_NULL(parent);
    ERR_FAIL_NULL(child->get_task());
    ERR_FAIL_NULL(parent->get_task());

    this->behaviour_tree->disconnect_tasks(parent->get_task(), child->get_task());
    this->graph_edit->disconnect_node(parent->get_name(), 0, child->get_name(), 0);
}

void BTGraphEditor::connection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port)
{
    godot::StringName parent_task_name = this->graph_view->get_task_name(_from_node);
    godot::StringName child_task_name = this->graph_view->get_task_name(_to_node);

    godot::Ref<BTTask> parent_task = this->behaviour_tree->get_task_by_name(parent_task_name);
    godot::Ref<BTTask> child_task = this->behaviour_tree->get_task_by_name(child_task_name);

    ERR_FAIL_COND_MSG(parent_task.is_null(), "\"" + parent_task_name + "\" custom task name is not in the behaviour tree.");
    ERR_FAIL_COND_MSG(child_task.is_null(), "\"" + child_task_name + "\" custom task name is not in the behaviour tree.");

    ERR_FAIL_COND_MSG(godot::Ref<BTSubtree>(parent_task).is_valid(), "Parent cannot be subtree.");

    bool can_connect = this->behaviour_tree->can_connect(parent_task, child_task);
    if (!can_connect)
    {
        return;
    }

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Create a connection.");
    
    godot::Vector<StringName> current_children_names = BTGraphEditor::bttask_array_to_names(parent_task->get_children());

    int index = this->graph_view->find_insert_index_by_y(child_task_name, current_children_names);

    undo_redo_manager->add_do_method(this->behaviour_tree, "connect_tasks", parent_task, child_task, index);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "disconnect_tasks", parent_task, child_task);

    undo_redo_manager->add_do_method(this->graph_view, "connect_task_nodes", parent_task_name, child_task_name);
    undo_redo_manager->add_undo_method(this->graph_edit, "disconnect_task_nodes", parent_task_name, child_task_name);

    undo_redo_manager->commit_action();
}

void BTGraphEditor::disconnection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port)
{
    godot::StringName parent_task_name = this->graph_view->get_task_name(_from_node);
    godot::StringName child_task_name = this->graph_view->get_task_name(_to_node);

    godot::Ref<BTTask> parent_task = this->behaviour_tree->get_task_by_name(parent_task_name);
    godot::Ref<BTTask> child_task = this->behaviour_tree->get_task_by_name(child_task_name);

    ERR_FAIL_COND_MSG(parent_task.is_null(), "\"" + parent_task_name + "\" custom task name is not in the behaviour tree.");
    ERR_FAIL_COND_MSG(child_task.is_null(), "\"" + child_task_name + "\" custom task name is not in the behaviour tree.");

    bool can_disconnect = this->behaviour_tree->can_disconnect(parent_task, child_task);
    if (!can_disconnect)
    {
        return;
    }
    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Remove a connection.");

    int index = parent_task->get_child_index(child_task);

    undo_redo_manager->add_do_method(this->behaviour_tree, "disconnect_tasks", parent_task, child_task);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "connect_tasks", parent_task, child_task, index);

    undo_redo_manager->add_do_method(this->graph_view, "disconnect_task_nodes", parent_task_name, child_task_name);
    undo_redo_manager->add_undo_method(this->graph_view, "connect_task_nodes", parent_task_name, child_task_name);

    undo_redo_manager->commit_action();
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
    if (this->behaviour_tree != nullptr)
    {
        this->save_tree();
    }
    this->behaviour_tree = new_tree;
    this->clear_graph_nodes();
    if (this->saved_trees.has(new_tree))
    {
        this->load_tree();
    }
    else
    {
        this->create_default_graph_nodes();
    }
    //TODO : make instance read_only if node is instanced scene.
}

void BTGraphEditor::_bind_methods()
{
    using namespace godot;

    // Setup Methods
    ClassDB::bind_method(D_METHOD("_setup_graph_edit"), &BTGraphEditor::_setup_graph_edit);
    ClassDB::bind_method(D_METHOD("_setup_task_names"), &BTGraphEditor::_setup_task_names);
    ClassDB::bind_method(D_METHOD("_setup_rename_edit"), &BTGraphEditor::_setup_rename_edit);
    ClassDB::bind_method(D_METHOD("_setup_path_edit"), &BTGraphEditor::_setup_path_edit);
    ClassDB::bind_method(D_METHOD("_setup_popup_menu"), &BTGraphEditor::_setup_popup_menu);
    ClassDB::bind_method(D_METHOD("_fill_action_condition_type_popup_menu", "action_condition"), &BTGraphEditor::_fill_action_condition_type_popup_menu);

    // Utility Methods
    ClassDB::bind_method(D_METHOD("connect_graph_node_signals"), &BTGraphEditor::connect_graph_node_signals);
    ClassDB::bind_method(D_METHOD("new_graph_node"), &BTGraphEditor::new_graph_node);
    ClassDB::bind_method(D_METHOD("new_graph_node_subtree"), &BTGraphEditor::new_graph_node_subtree);
    ClassDB::bind_method(D_METHOD("new_bt_graph_node_from_task", "bt_task"), &BTGraphEditor::new_bt_graph_node_from_task);
    ClassDB::bind_method(D_METHOD("new_bt_graph_node_subtree_from_task", "bt_subtree"), &BTGraphEditor::new_bt_graph_node_subtree_from_task);
    ClassDB::bind_method(D_METHOD("get_graph_nodes"), &BTGraphEditor::get_graph_nodes);
    ClassDB::bind_method(D_METHOD("get_sorted_by_y_children_of_parent", "parent_graph_node"), &BTGraphEditor::get_sorted_by_y_children_of_parent);
    ClassDB::bind_method(D_METHOD("get_node_insert_index_by_y_in_children", "parent_graph_node", "graph_node"), &BTGraphEditor::get_node_insert_index_by_y_in_children);

    /* can't pass Vector<>& check.*/
    /*ClassDB::bind_method(D_METHOD("_extract_node_levels_into_stack", "root_node", "stack", "current_level"), &BTGraphEditor::_extract_node_levels_into_stack);*/

    // Node Management
    ClassDB::bind_method(D_METHOD("insert_node", "bt_graph_node"), &BTGraphEditor::insert_node);
    ClassDB::bind_method(D_METHOD("erase_node", "bt_graph_node"), &BTGraphEditor::erase_node);
    /*ClassDB::bind_method(D_METHOD("delete_nodes", "nodes_to_delete"), &BTGraphEditor::delete_nodes);*/
    ClassDB::bind_method(D_METHOD("clear_graph_nodes"), &BTGraphEditor::clear_graph_nodes);
    ClassDB::bind_method(D_METHOD("create_default_graph_nodes"), &BTGraphEditor::create_default_graph_nodes);
    ClassDB::bind_method(D_METHOD("set_root_node", "new_root_node"), &BTGraphEditor::set_root_node);
    ClassDB::bind_method(D_METHOD("arrange_nodes", "with_undo_redo"), &BTGraphEditor::arrange_nodes);
    ClassDB::bind_method(D_METHOD("color_root_node"), &BTGraphEditor::color_root_node);
    ClassDB::bind_method(D_METHOD("deselect_all_nodes"), &BTGraphEditor::deselect_all_nodes);
    ClassDB::bind_method(D_METHOD("name_node"), &BTGraphEditor::name_node);
    ClassDB::bind_method(D_METHOD("save_tree"), &BTGraphEditor::save_tree);
    ClassDB::bind_method(D_METHOD("load_tree"), &BTGraphEditor::load_tree);

    // Drag and Drop
    ClassDB::bind_method(D_METHOD("_node_dragged", "from", "to", "node_name"), &BTGraphEditor::_node_dragged);
    ClassDB::bind_method(D_METHOD("_move_nodes"), &BTGraphEditor::_move_nodes);

    // Event Handlers
    ClassDB::bind_method(D_METHOD("_add_new_node_button_pressed"), &BTGraphEditor::_add_new_node_button_pressed);
    ClassDB::bind_method(D_METHOD("_add_new_subtree_node_button_pressed"), &BTGraphEditor::_add_new_subtree_node_button_pressed);
    ClassDB::bind_method(D_METHOD("_arrange_nodes_button_pressed"), &BTGraphEditor::_arrange_nodes_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_rename_edit_text_submitted", "new_text"), &BTGraphEditor::_on_rename_edit_text_submitted);
    ClassDB::bind_method(D_METHOD("_on_rename_edit_focus_exited"), &BTGraphEditor::_on_rename_edit_focus_exited);
    ClassDB::bind_method(D_METHOD("_on_node_selected", "selected_node"), &BTGraphEditor::_on_node_selected);
    ClassDB::bind_method(D_METHOD("_on_node_deselected", "deselected_node"), &BTGraphEditor::_on_node_deselected);
    ClassDB::bind_method(D_METHOD("_on_path_edit_text_submitted", "new_path"), &BTGraphEditor::_on_path_edit_text_submitted);
    ClassDB::bind_method(D_METHOD("_on_path_edit_focus_exited"), &BTGraphEditor::_on_path_edit_focus_exited);
    ClassDB::bind_method(D_METHOD("_on_node_double_clicked", "clicked_node"), &BTGraphEditor::_on_node_double_clicked);
    ClassDB::bind_method(D_METHOD("_on_node_right_clicked", "clicked_node"), &BTGraphEditor::_on_node_right_clicked);
    ClassDB::bind_method(D_METHOD("_on_node_subtree_double_clicked", "clicked_node"), &BTGraphEditor::_on_node_subtree_double_clicked);
    ClassDB::bind_method(D_METHOD("_on_node_subtree_right_clicked", "clicked_node"), &BTGraphEditor::_on_node_subtree_right_clicked);
    ClassDB::bind_method(D_METHOD("_on_main_popup_menu_item_selected", "id"), &BTGraphEditor::_on_main_popup_menu_item_selected);
    ClassDB::bind_method(D_METHOD("_on_task_type_popup_menu_item_selected", "id"), &BTGraphEditor::_on_task_type_popup_menu_item_selected);
    ClassDB::bind_method(D_METHOD("_on_action_condition_type_popup_menu_item_selected", "id"), &BTGraphEditor::_on_action_condition_type_popup_menu_item_selected);
    ClassDB::bind_method(D_METHOD("_on_action_condition_type_popup_menu_show", "action_condtition"), &BTGraphEditor::_on_action_condition_type_popup_menu_show);
    ClassDB::bind_method(D_METHOD("_delete_nodes_request", "nodes_to_delete"), &BTGraphEditor::_delete_nodes_request);

    // Copy Paste Handling
    ClassDB::bind_method(D_METHOD("copy_nodes_request"), &BTGraphEditor::copy_nodes_request);
    ClassDB::bind_method(D_METHOD("paste_nodes_request"), &BTGraphEditor::paste_nodes_request);
    ClassDB::bind_method(D_METHOD("clear_copied_nodes"), &BTGraphEditor::clear_copied_nodes);

    // Connection Handling
    ClassDB::bind_method(D_METHOD("_connect_nodes", "parent_node" "child_node"), &BTGraphEditor::_connect_nodes);
    ClassDB::bind_method(D_METHOD("_disconnect_nodes", "parent_node" "child_node"), &BTGraphEditor::_disconnect_nodes);
    ClassDB::bind_method(D_METHOD("connection_request", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::connection_request);
    ClassDB::bind_method(D_METHOD("disconnection_request", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::disconnection_request);

    // Task Management
    ClassDB::bind_method(D_METHOD("change_task_type", "class_name", "node"), &BTGraphEditor::change_task_type);

    // Getters and Setters
    ClassDB::bind_method(D_METHOD("set_editor_plugin", "editor_plugin"), &BTGraphEditor::set_editor_plugin);
    // ClassDB::bind_method(D_METHOD("get_graph_edit"), &BTGraphEditor::get_graph_edit);
    BIND_GETTER_SETTER_DEFAULT(BTGraphEditor, behaviour_tree);
}