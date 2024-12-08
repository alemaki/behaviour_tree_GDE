#include "bt_graph_editor.hpp"
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/editor_interface.hpp>

#include "behaviour_tree/utils/utils.hpp"

BTGraphEditor::BTGraphEditor()
{
    this->_setup_graph_view();
    this->_setup_task_names();
    this->_setup_rename_edit();
    this->_setup_path_edit();
    this->_setup_popup_menu();

    this->drag_called = false;
}

BTGraphEditor::~BTGraphEditor()
{
    //this->clear_copied_tasks();
    memdelete(this->rename_edit);
    memdelete(this->path_edit);
    memdelete(this->main_popup_menu);
    memdelete(this->graph_view);
}

/* Setup Methods */
void BTGraphEditor::_setup_graph_view()
{
    this->graph_view = memnew(BTGraphView);

    this->graph_view->call_deferred("connect", "connection_request", callable_mp(this, &BTGraphEditor::connection_request));
    this->graph_view->call_deferred("connect", "disconnection_request", callable_mp(this, &BTGraphEditor::disconnection_request));
    this->graph_view->call_deferred("connect", "delete_nodes_request", callable_mp(this, &BTGraphEditor::_delete_nodes_request));
    this->graph_view->call_deferred("connect", "copy_nodes_request", callable_mp(this, &BTGraphEditor::copy_nodes_request));
    this->graph_view->call_deferred("connect", "paste_nodes_request", callable_mp(this, &BTGraphEditor::paste_nodes_request));
}

void BTGraphEditor::_setup_task_names()
{
    this->composite_names = utils::get_subclasses(BTComposite::get_class_static());
    this->decorator_names = utils::get_subclasses(BTDecorator::get_class_static());
}

void BTGraphEditor::_setup_rename_edit()
{
    ERR_FAIL_COND(this->graph_view == nullptr);

    this->rename_edit = memnew(godot::LineEdit);
    this->rename_edit->set_visible(false);
    this->graph_view->add_child(this->rename_edit);

    this->rename_edit->call_deferred("connect", "text_submitted", callable_mp(this, &BTGraphEditor::_on_rename_edit_text_submitted));
    this->rename_edit->call_deferred("connect", "focus_exited", callable_mp(this, &BTGraphEditor::_on_rename_edit_focus_exited));
}

void BTGraphEditor::_setup_path_edit()
{
    ERR_FAIL_COND(this->graph_view == nullptr);

    this->path_edit = memnew(godot::LineEdit);
    this->path_edit->set_visible(false);
    this->graph_view->add_child(this->path_edit);

    this->path_edit->call_deferred("connect", "text_submitted", callable_mp(this, &BTGraphEditor::_on_path_edit_text_submitted));
    this->path_edit->call_deferred("connect", "focus_exited", callable_mp(this, &BTGraphEditor::_on_path_edit_focus_exited));
}

void BTGraphEditor::_setup_popup_menu()
{
    ERR_FAIL_COND(this->graph_view == nullptr);

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

    this->graph_view->add_child(this->main_popup_menu);

    this->task_type_popup_menu->call_deferred("connect", "id_pressed", callable_mp(this, &BTGraphEditor::_on_task_type_popup_menu_item_selected));
    this->main_popup_menu->call_deferred("connect", "id_pressed", callable_mp(this, &BTGraphEditor::_on_main_popup_menu_item_selected));

    this->action_condition_type_popup_menu = memnew(godot::PopupMenu);
    this->action_condition_type_popup_menu->set_allow_search(true);
    this->graph_view->add_child(this->action_condition_type_popup_menu);

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

void BTGraphEditor::connect_graph_node_signals(const godot::StringName& task_name)
{
    BTGraphNode* node = this->graph_view->get_graph_node(task_name);
    ERR_FAIL_COND(node == nullptr);
    node->call_deferred("connect", "dragged", callable_mp(this, &BTGraphEditor::_node_dragged).bind(task_name));
    node->call_deferred("connect", "node_selected", callable_mp(this, &BTGraphEditor::_on_node_selected).bind(task_name));
    node->call_deferred("connect", "node_deselected", callable_mp(this, &BTGraphEditor::_on_node_deselected).bind(task_name));

    if (godot::Object::cast_to<BTGraphNodeSubtree>(node) != nullptr)
    {
        node->call_deferred("connect", "double_clicked", callable_mp(this, &BTGraphEditor::_on_node_subtree_double_clicked).bind(task_name));
        node->call_deferred("connect", "right_clicked", callable_mp(this, &BTGraphEditor::_on_node_subtree_right_clicked).bind(task_name));
    }
    else
    {
        node->call_deferred("connect", "double_clicked", callable_mp(this, &BTGraphEditor::_on_node_double_clicked).bind(task_name));
        node->call_deferred("connect", "right_clicked", callable_mp(this, &BTGraphEditor::_on_node_right_clicked).bind(task_name));
    }
}

godot::String get_name_of_tree(BehaviourTree* tree)
{
    return godot::String("BehaviourTree_") + godot::itos(tree->get_instance_id());
}

void BTGraphEditor::save_tree()
{
    ERR_FAIL_NULL(this->behaviour_tree);
    godot::StringName behaviour_tree_save_name = get_name_of_tree(this->behaviour_tree);
    this->graph_view->clear_and_save_graph(behaviour_tree_save_name);
    this->saved_trees.insert(this->behaviour_tree);
}

void BTGraphEditor::load_tree()
{
    ERR_FAIL_NULL(this->behaviour_tree);
    ERR_FAIL_COND(!(this->saved_trees.has(this->behaviour_tree)));
    godot::StringName behaviour_tree_save_name = get_name_of_tree(this->behaviour_tree);
    this->graph_view->load_graph(behaviour_tree_save_name);
    this->saved_trees.erase(this->behaviour_tree);
    this->color_root_node();
}


/* Node Management */

void BTGraphEditor::delete_nodes(const godot::Vector<StringName>& task_names_to_delete)
{
    ERR_FAIL_NULL(this->behaviour_tree);
    int size = task_names_to_delete.size();

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Delete node(s)");

    for (int i = 0; i < size; i++)
    {
        godot::Ref<BTTask> task_to_remove = this->behaviour_tree->get_task_by_name(task_names_to_delete[i]);
        godot::StringName task_to_remove_name = task_to_remove->get_name();
        godot::Array task_children = task_to_remove->get_children();
        godot::Ref<BTTask> parent = task_to_remove->get_parent();
        BTGraphNode* node_to_delete = graph_view->get_graph_node(task_to_remove_name);
        int task_id = this->behaviour_tree->get_task_id(task_to_remove);

        undo_redo_manager->add_do_method(this->behaviour_tree, "set_children_of_task", task_to_remove, godot::Array());
        undo_redo_manager->add_do_method(this->behaviour_tree, "remove_task_by_ref", task_to_remove);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "add_task", task_id, task_to_remove);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "set_children_of_task", task_to_remove, task_children);

        /* add node first when undoing so connections can form. No need to disconnect them later because deleting will remove connection.*/
        undo_redo_manager->add_undo_method(this->graph_view, "create_task_node", task_to_remove_name, task_to_remove->get_class_static());
        undo_redo_manager->add_undo_method(this->graph_view, "set_node_position", task_to_remove_name, node_to_delete->get_position_offset());
        undo_redo_manager->add_undo_method(this->graph_view, "set_task_node_title", task_to_remove_name, task_to_remove->get_custom_name());

        for (int j = 0; j < task_children.size(); j++)
        {
            godot::Ref<BTTask> child = task_children[j];
            undo_redo_manager->add_undo_method(this->graph_view, "connect_task_nodes", task_to_remove->get_name(), child->get_name());
        }

        if (parent.is_valid())
        {
            int index = parent->get_child_index(task_to_remove);
            undo_redo_manager->add_undo_method(this->behaviour_tree, "connect_tasks", parent, task_to_remove, index);
            undo_redo_manager->add_do_method(this->graph_view, "disconnect_task_nodes", parent->get_name(), task_to_remove->get_name());
            undo_redo_manager->add_undo_method(this->graph_view, "connect_task_nodes", parent->get_name(), task_to_remove->get_name());
        }

        if (this->behaviour_tree->get_root_task() == task_to_remove)
        {
            undo_redo_manager->add_undo_method(this->behaviour_tree, "set_root_task", task_to_remove);
        }
        /* remove node last safely */
        undo_redo_manager->add_do_method(this->graph_view, "delete_task_node", task_to_remove->get_name());
        undo_redo_manager->add_do_method(this, "color_root_node");

        undo_redo_manager->add_undo_method(this, "color_root_node");
    }

    undo_redo_manager->commit_action();
}

void BTGraphEditor::create_default_graph_nodes()
{
    godot::Array tasks_array = this->behaviour_tree->get_tasks();
    for (int i = 0; i < tasks_array.size(); i++)
    {
        godot::Ref<BTTask> task = tasks_array[i];
        this->graph_view->create_task_node(task->get_name(), task->get_class());
        if (godot::Ref<BTSubtree>(task).is_valid())
        {
            this->graph_view->set_task_node_title(task->get_name(), godot::Ref<BTSubtree>(task)->get_file_path());
        }
        else
        {
            this->graph_view->set_task_node_title(task->get_name(), task->get_custom_name());
        }
        this->connect_graph_node_signals(task->get_name());
    }

    this->arrange_nodes();

    for (int i = 0; i < tasks_array.size(); i++)
    {
        godot::Ref<BTTask> task = tasks_array[i];
        godot::Array children = task->get_children();
        for (int j = 0; j < children.size(); j++)
        {
            godot::Ref<BTTask> child = children[j];
            this->graph_view->connect_task_nodes(task->get_name(), child->get_name());
        }
    }
    this->color_root_node();
}

void BTGraphEditor::set_root_node(const godot::StringName& task_name)
{
    godot::Ref<BTTask> current_root = this->behaviour_tree->get_root_task();
    ERR_FAIL_COND(current_root.is_null());

    if (task_name == current_root->get_name())
    {
        return;
    }

    godot::Ref<BTTask> old_root = this->behaviour_tree->get_root_task();
    godot::Ref<BTTask> new_root = this->behaviour_tree->get_task_by_name(task_name);

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Set root node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "set_root_task", new_root);
    undo_redo_manager->add_do_method(this, "color_root_node");

    undo_redo_manager->add_undo_method(this->behaviour_tree, "set_root_task", old_root);
    undo_redo_manager->add_undo_method(this, "color_root_node");

    undo_redo_manager->commit_action();
}

godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> get_node_map(godot::Ref<BTTask> root_task)
{
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
        task_queue_i++;
    }
    return parent_to_child_names;
}

void BTGraphEditor::arrange_nodes(bool with_undo_redo)
{
    if (!(this->behaviour_tree->get_root_task().is_valid()))
    {
        return;
    }

    godot::Ref<BTTask> root_task = this->behaviour_tree->get_root_task();
    godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_child_names = get_node_map(root_task);

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
    ERR_FAIL_NULL(this->behaviour_tree);
    godot::Ref<BTTask> root = this->behaviour_tree->get_root_task();
    if (root.is_null())
    {
        return;
    }
    graph_view->set_root_task_name(root->get_name());
}

// void BTGraphEditor::deselect_all_nodes()
// {
//     for (godot::KeyValue<godot::Ref<BTTask>, BTGraphNode*> element : task_to_node)
//     {
//         ERR_CONTINUE(element.value == nullptr);
//         ERR_CONTINUE(element.value->get_task().is_null());
//         if (element.value->is_selected())
//         {
//             element.value->set_selected(false);
//         }
//     }
// }

/* Drag and Drop */

void BTGraphEditor::_node_dragged(const godot::Vector2 &_from, const godot::Vector2 &_to, const godot::StringName& task_name)
{
    this->drag_buffer.push_back({_from, _to, task_name});
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
        godot::StringName task_name =operation.task_name;
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
        undo_redo->add_do_method(this->graph_view, "set_node_position", operation.task_name, operation.to_position);
        undo_redo->add_undo_method(this->graph_view, "set_node_position", operation.task_name, operation.from_position);
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
    undo_redo_manager->add_do_method(this, "color_root_node");
    undo_redo_manager->add_do_method(this, "connect_graph_node_signals", task_name);

    undo_redo_manager->add_undo_method(this->graph_view, "delete_task_node", task_name);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task", id);
    undo_redo_manager->add_undo_method(this, "color_root_node");

    undo_redo_manager->commit_action();
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
    undo_redo_manager->add_do_method(this, "color_root_node");
    undo_redo_manager->add_do_method(this, "connect_graph_node_signals", task_name);

    undo_redo_manager->add_undo_method(this->graph_view, "delete_task_node", task_name);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task", id);
    undo_redo_manager->add_undo_method(this, "color_root_node");

    undo_redo_manager->commit_action();

    //this->connect_graph_node_signals(bt_graph_node);
}

void BTGraphEditor::_arrange_nodes_button_pressed()
{
    this->arrange_nodes(true);
}

void BTGraphEditor::_on_rename_edit_text_submitted(const godot::String& new_text)
{
    ERR_FAIL_COND(this->last_double_clicked_node.is_empty());
    ERR_FAIL_COND_EDMSG(new_text == "", "Name of node must not be empty.");

    BTGraphNode* node = this->graph_view->get_graph_node(last_double_clicked_node);
    const godot::String& old_text = node->get_title();
    godot::Ref<BTTask> task = this->behaviour_tree->get_task_by_name(last_double_clicked_node);

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Rename node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "set_custom_name_task_by_ref", task, new_text);
    undo_redo_manager->add_do_method(this->graph_view, "set_task_node_title", task->get_name(), new_text);

    undo_redo_manager->add_undo_method(this->graph_view, "set_task_node_title", task->get_name(), old_text);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "set_custom_name_task_by_ref", task, old_text);

    undo_redo_manager->commit_action();

    this->rename_edit->set_visible(false);
}

void BTGraphEditor::_on_rename_edit_focus_exited()
{
    this->rename_edit->set_visible(false);
}

void BTGraphEditor::_on_path_edit_text_submitted(const godot::String& new_path)
{
    ERR_FAIL_COND(this->last_double_clicked_node.is_empty());
    BTGraphNodeSubtree* node = godot::Object::cast_to<BTGraphNodeSubtree>(this->graph_view->get_graph_node(last_double_clicked_node));
    ERR_FAIL_NULL(node);
    ERR_FAIL_COND_EDMSG(new_path == "", "Path must not be empty.");

    godot::Ref<BTSubtree> subtree_task = this->behaviour_tree->get_task_by_name(last_double_clicked_node);
    const godot::String& old_path = subtree_task->get_file_path();

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Change path of subtree.");

    /* register changes on scene. TODO: fix */
    undo_redo_manager->add_do_method(this->behaviour_tree, "set_root_task", this->behaviour_tree->get_root_task());
    undo_redo_manager->add_do_method(subtree_task.ptr(), "set_file_path", new_path);
    undo_redo_manager->add_do_method(node, "set_file_path", new_path);
    
    undo_redo_manager->add_undo_method(subtree_task.ptr(), "set_file_path", old_path);
    undo_redo_manager->add_undo_method(node, "set_file_path", old_path);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "set_root_task", this->behaviour_tree->get_root_task());

    undo_redo_manager->commit_action();

    this->rename_edit->set_visible(false);
}

void BTGraphEditor::_on_path_edit_focus_exited()
{
    this->path_edit->set_visible(false);
}

void BTGraphEditor::_on_node_selected(const godot::StringName& task_name)
{
    godot::Ref<BTTask> task = this->behaviour_tree->get_task_by_name(task_name);
    ERR_FAIL_COND(task.is_null());
    godot::EditorInterface* editor_interface = godot::EditorInterface::get_singleton();
    editor_interface->inspect_object(task.ptr());
}

void BTGraphEditor::_on_node_deselected(const godot::StringName& _task_name)
{
    ERR_FAIL_COND(this->behaviour_tree == nullptr);

    godot::EditorInterface* editor_interface = godot::EditorInterface::get_singleton();
    editor_interface->inspect_object(this->behaviour_tree);
}

void BTGraphEditor::_on_node_double_clicked(const godot::StringName& task_name)
{
    ERR_FAIL_COND(!(this->graph_view->has_task_name(task_name)));
    this->last_double_clicked_node = task_name;
    BTGraphNode* clicked_node = this->graph_view->get_graph_node(task_name);
    this->rename_edit->set_text(clicked_node->get_title());
    this->rename_edit->set_visible(true);
    /* grab_focus should be a deferred call. See:
     * https://docs.godotengine.org/en/stable/classes/class_control.html#class-control-method-grab-focus */
    if (this->rename_edit->is_inside_tree())
    {
        this->rename_edit->call_deferred("grab_focus");
    }
    this->rename_edit->set_size(godot::Size2(clicked_node->get_size().x, this->path_edit->get_size().y));
    this->rename_edit->set_position(godot::Vector2(clicked_node->get_position().x, 
                                                   clicked_node->get_position().y - this->rename_edit->get_size().y));
    
    /* show up on front not behind */
    this->rename_edit->set_z_index(clicked_node->get_z_index() + 1);
}

void BTGraphEditor::_on_node_right_clicked(const godot::StringName& task_name)
{
    ERR_FAIL_COND(!(this->graph_view->has_task_name(task_name)));

    this->last_right_clicked_node = task_name;
    BTGraphNode* clicked_node = this->graph_view->get_graph_node(task_name);

    godot::Vector2 menu_position = clicked_node->get_global_position();
    int current_screen = clicked_node->get_viewport()->get_window()->get_current_screen();

    menu_position.y += clicked_node->get_size().y;

    this->main_popup_menu->set_visible(true);
    this->main_popup_menu->set_position(menu_position);
    this->main_popup_menu->set_current_screen(current_screen);
    this->main_popup_menu->call_deferred("grab_focus");

    this->_on_node_selected(task_name);
    
    /* TODO: find better solution. */
    int size = this->composite_names.size() + this->decorator_names.size();
    for (int i = 0; i < size; i++)
    {
        this->task_type_popup_menu->set_item_checked(i, false);
    }

    godot::Ref<BTTask> task = this->behaviour_tree->get_task_by_name(task_name);
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

void BTGraphEditor::_on_node_subtree_double_clicked(const godot::StringName& task_name)
{
    ERR_FAIL_COND(!(this->graph_view->has_task_name(task_name)));
    BTGraphNodeSubtree* clicked_node = godot::Object::cast_to<BTGraphNodeSubtree>(this->graph_view->get_graph_node(task_name));
    ERR_FAIL_NULL(clicked_node);

    this->last_double_clicked_node = task_name;
    this->path_edit->set_text(clicked_node->get_file_path());
    this->path_edit->set_visible(true);
    this->path_edit->call_deferred("grab_focus");
    this->path_edit->set_size(godot::Size2(clicked_node->get_size().x, this->path_edit->get_size().y));
    this->path_edit->set_position(godot::Vector2(clicked_node->get_position().x, 
                                                   clicked_node->get_position().y - this->path_edit->get_size().y));
    
    this->path_edit->set_z_index(clicked_node->get_z_index() + 1);
}

void BTGraphEditor::_on_node_subtree_right_clicked(const godot::StringName& task_name)
{
    //TODO?
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
        this->delete_nodes({});
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
        this->_change_task_type(item_text, this->last_right_clicked_node);
    }
}


void BTGraphEditor::_on_action_condition_type_popup_menu_show(const godot::StringName& action_condition)
{
    ERR_FAIL_COND(this->graph_view->has_task_name(this->last_right_clicked_node));
    /* Refill every time because user might define new subclasses*/
    this->_fill_action_condition_type_popup_menu(action_condition);
    BTGraphNode* node = this->graph_view->get_graph_node(this->last_right_clicked_node);

    godot::Vector2 menu_position = node->get_global_position();
    int current_screen = node->get_viewport()->get_window()->get_current_screen();

    menu_position.y += node->get_size().y;

    this->action_condition_type_popup_menu->set_visible(true);
    this->action_condition_type_popup_menu->set_position(menu_position);
    this->action_condition_type_popup_menu->set_current_screen(current_screen);
    this->action_condition_type_popup_menu->call_deferred("grab_focus");
}

void BTGraphEditor::_on_action_condition_type_popup_menu_item_selected(int id)
{
    godot::StringName item = this->action_condition_type_popup_menu->get_item_text(id);
    this->_change_task_type(item, this->last_right_clicked_node);
}

void BTGraphEditor::_delete_nodes_request(godot::TypedArray<godot::StringName> _node_names_to_delete)
{
    int size = _node_names_to_delete.size();

    if (size == 0)
    {
        return;
    }

    /* Honestly, the best solution I can think of right now is to just disable it,
     * because undoing all the deleted connections will be a pain.
     * https://en.wikipedia.org/wiki/Ostrich_algorithm .*/
    ERR_FAIL_COND_EDMSG(size != 1, "Cannot delete mutliple nodes.");

    godot::Vector<StringName> task_names_to_delete;
    task_names_to_delete.resize(size);

    for (int i = 0; i < size; i++)
    {
        godot::StringName task_name = this->graph_view->get_task_name(_node_names_to_delete[i]);
        task_names_to_delete.set(i, task_name);
    }

    this->delete_nodes(task_names_to_delete);
    
}


/* Copy-pasta Handling */

void BTGraphEditor::copy_nodes_request()
{
    /* Copied/selected nodes might be deleted later by user. So create copies now while still valid. */

    godot::Vector<godot::StringName> selected_nodes = graph_view->get_selected_node_task_names();

    godot::HashSet<godot::Ref<BTTask>> copied_tasks;
    godot::HashMap<godot::Ref<BTTask>, godot::Vector2> copied_positions;
    godot::HashMap<godot::Ref<BTTask>, godot::Vector<godot::Ref<BTTask>>> parent_to_children_copied_connections;

    for (const godot::StringName& task_name : selected_nodes)
    {
        godot::Ref<BTTask> task = this->behaviour_tree->get_task_by_name(task_name);
        ERR_FAIL_COND(task.is_null());

        copied_tasks.insert(task);

        godot::Vector2 postion_offset = this->graph_view->get_graph_node(task->get_name())->get_position_offset();
        copied_positions.insert(task, postion_offset);
    }

    for (godot::Ref<BTTask> task : copied_tasks)
    {
        godot::Vector<godot::Ref<BTTask>> copy_children;
        godot::Array children = task->get_children();
        for (int i = 0, size = children.size(); i < size; i++)
        {
            godot::Ref<BTTask> child = children[i];
            if (copied_tasks.has(child))
            {
                copy_children.push_back(child);
            }
        }
        if (copy_children.size() > 0)
        {
            parent_to_children_copied_connections.insert(task, copy_children);
        }
    }

    this->clear_copied_tasks();

    this->copy_info = {
        copied_tasks,
        parent_to_children_copied_connections,
        copied_positions
    };
}

void BTGraphEditor::paste_nodes_request()
{
    if (this->copy_info.copied_tasks.size() == 0)
    {
        return;
    }

    godot::HashMap<godot::Ref<BTTask>, godot::Ref<BTTask>> copied_to_pasted;
    godot::HashMap<godot::Ref<BTTask>, godot::Vector<godot::Ref<BTTask>>> pasted_to_pasted_children_connections;

    for (godot::Ref<BTTask> task : copy_info.copied_tasks)
    {
        godot::Ref<BTTask> copy = task->copy();
        copied_to_pasted.insert(task, copy);
    }

    for (const godot::KeyValue<godot::Ref<BTTask>, godot::Vector<godot::Ref<BTTask>>>& task_copy_to_children : copy_info.copied_connections)
    {
        godot::Vector<godot::Ref<BTTask>> pasted_children;
        godot::Ref<BTTask> old = task_copy_to_children.key;
        godot::Ref<BTTask> pasted_task = copied_to_pasted[old];

        for (godot::Ref<BTTask> old_child : task_copy_to_children.value)
        {
            pasted_children.push_back(copied_to_pasted[old_child]);
        }
        pasted_to_pasted_children_connections.insert(pasted_task, pasted_children);
    }

    EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Paste copied nodes");

    /* Paste all nodes first so connections can be created later. */
    for (const godot::KeyValue<godot::Ref<BTTask>, godot::Ref<BTTask>>& old_to_paste : copied_to_pasted)
    {
        // TODO: Extract logic for adding new node to the graph.
        godot::Ref<BTTask> task_to_paste = old_to_paste.value;
        godot::StringName task_custom_name = task_to_paste->get_custom_name();
        godot::StringName task_name = task_to_paste->get_name();

        undo_redo_manager->add_do_method(this->behaviour_tree, "add_task_by_ref", task_to_paste);
        undo_redo_manager->add_do_method(this->graph_view, "create_task_node", task_name);
        undo_redo_manager->add_do_method(this->graph_view, "set_task_node_title", task_name, task_custom_name);
        undo_redo_manager->add_do_method(this->graph_view, "change_task_class_name", task_name, task_to_paste->get_class());
        undo_redo_manager->add_do_method(this->graph_view, "set_node_position", task_name, copy_info.copied_positions[old_to_paste.key]);
        undo_redo_manager->add_do_method(this, "connect_graph_node_signals", task_name);
    }

    undo_redo_manager->add_do_method(this->graph_view, "deselect_all_nodes");

    for (const godot::KeyValue<godot::Ref<BTTask>, godot::Ref<BTTask>>& old_to_paste : copied_to_pasted)
    {
        /* Select pasted nodes for easy interaction */
        undo_redo_manager->add_do_method(this->graph_view, "set_task_node_selected", old_to_paste.value->get_name(), true);
    }

    for (const godot::KeyValue<godot::Ref<BTTask>, godot::Vector<godot::Ref<BTTask>>>& pasted_to_pasted_children : pasted_to_pasted_children_connections)
    {
        godot::Ref<BTTask> pasted = pasted_to_pasted_children.key;
        const godot::Vector<godot::Ref<BTTask>>& pasted_children = pasted_to_pasted_children.value;
        
        for (int i = 0; i < pasted_children.size(); i++)
        {
            godot::Ref<BTTask> pasted_child = pasted_children[i];
            undo_redo_manager->add_do_method(this->behaviour_tree, "connect_tasks", pasted, pasted_child, i);
            undo_redo_manager->add_undo_method(this->behaviour_tree, "disconnect_tasks", pasted, pasted_child);
        
            undo_redo_manager->add_do_method(this->graph_view, "connect_task_nodes", pasted->get_name(), pasted_child->get_name());
            undo_redo_manager->add_undo_method(this->graph_view, "disconnect_task_nodes", pasted->get_name(), pasted_child->get_name());
        }
    }

    /* Add undo node from graph */
    for (const godot::KeyValue<godot::Ref<BTTask>, godot::Ref<BTTask>>& old_to_paste : copied_to_pasted)
    {
        godot::Ref<BTTask> task_to_paste = old_to_paste.value;
        godot::StringName task_name = task_to_paste->get_name();

        undo_redo_manager->add_undo_method(this->graph_view, "delete_task_node", task_name);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task_by_ref", task_to_paste);
        undo_redo_manager->add_undo_method(this, "color_root_node");
    }

    undo_redo_manager->add_do_method(this, "color_root_node");
    
    undo_redo_manager->commit_action();
}

void BTGraphEditor::clear_copied_tasks()
{
    this->copy_info.copied_connections.clear();
    this->copy_info.copied_tasks.clear();
    this->copy_info.copied_positions.clear();
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
    undo_redo_manager->add_undo_method(this->graph_view, "disconnect_task_nodes", parent_task_name, child_task_name);

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

void BTGraphEditor::_change_task_type(const godot::StringName& class_name, const godot::StringName& task_name)
{
    godot::Ref<BTTask> old_task = this->behaviour_tree->get_task_by_name(task_name);
    ERR_FAIL_COND_MSG(old_task.is_null(), "No task in BT named: " + task_name + ".");

    if (old_task->get_class() == class_name)
    {
        return;
    }

    ERR_FAIL_COND(!(this->graph_view->has_task_name(task_name)));
    BTGraphNode* node = this->graph_view->get_graph_node(task_name);

    godot::Ref<BTTask> new_task = godot::ClassDB::instantiate(class_name);
    
    ERR_FAIL_COND(new_task.is_null());
    
    new_task->set_custom_name(old_task->get_custom_name());
    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Change task type.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "swap_task_in", old_task, new_task);
    undo_redo_manager->add_do_method(this->graph_view, "change_task_class_name", task_name, class_name);
    undo_redo_manager->add_do_method(this->graph_view, "change_task_name", task_name, new_task->get_name());

    undo_redo_manager->add_undo_method(this->graph_view, "change_task_name", new_task->get_name(), task_name);
    undo_redo_manager->add_undo_method(this->graph_view, "change_task_class_name", task_name, old_task->get_class());
    undo_redo_manager->add_undo_method(this->behaviour_tree, "swap_task_in", new_task, old_task);

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
    if (this->behaviour_tree && !this->saved_trees.has(this->behaviour_tree))
    {
        this->save_tree();
    }
    this->behaviour_tree = new_tree;
    if (this->saved_trees.has(new_tree))
    {
        this->load_tree();
    }
    else
    {
        this->create_default_graph_nodes();
    }
    //TODO!!! : make instance read_only if node is instanced scene.
}

void BTGraphEditor::_bind_methods()
{
    using namespace godot;

    // Setup Methods
    ClassDB::bind_method(D_METHOD("_setup_graph_edit"), &BTGraphEditor::_setup_graph_view);
    ClassDB::bind_method(D_METHOD("_setup_task_names"), &BTGraphEditor::_setup_task_names);
    ClassDB::bind_method(D_METHOD("_setup_rename_edit"), &BTGraphEditor::_setup_rename_edit);
    ClassDB::bind_method(D_METHOD("_setup_path_edit"), &BTGraphEditor::_setup_path_edit);
    ClassDB::bind_method(D_METHOD("_setup_popup_menu"), &BTGraphEditor::_setup_popup_menu);
    ClassDB::bind_method(D_METHOD("_fill_action_condition_type_popup_menu", "action_condition"), &BTGraphEditor::_fill_action_condition_type_popup_menu);

    // Utility Methods
    ClassDB::bind_method(D_METHOD("connect_graph_node_signals"), &BTGraphEditor::connect_graph_node_signals);

    // Node Management
    /*ClassDB::bind_method(D_METHOD("delete_nodes", "nodes_to_delete"), &BTGraphEditor::delete_nodes);*/
    ClassDB::bind_method(D_METHOD("create_default_graph_nodes"), &BTGraphEditor::create_default_graph_nodes);
    ClassDB::bind_method(D_METHOD("set_root_node", "new_root_node"), &BTGraphEditor::set_root_node);
    ClassDB::bind_method(D_METHOD("arrange_nodes", "with_undo_redo"), &BTGraphEditor::arrange_nodes);
    ClassDB::bind_method(D_METHOD("color_root_node"), &BTGraphEditor::color_root_node);
    //ClassDB::bind_method(D_METHOD("deselect_all_nodes"), &BTGraphEditor::deselect_all_nodes);
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
    ClassDB::bind_method(D_METHOD("clear_copied_tasks"), &BTGraphEditor::clear_copied_tasks);

    // Connection Handling
    ClassDB::bind_method(D_METHOD("connection_request", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::connection_request);
    ClassDB::bind_method(D_METHOD("disconnection_request", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::disconnection_request);

    // Task Management
    ClassDB::bind_method(D_METHOD("_change_task_type", "class_name", "node"), &BTGraphEditor::_change_task_type);

    // Getters and Setters
    ClassDB::bind_method(D_METHOD("set_editor_plugin", "editor_plugin"), &BTGraphEditor::set_editor_plugin);
    // ClassDB::bind_method(D_METHOD("get_graph_edit"), &BTGraphEditor::get_graph_edit);
    BIND_GETTER_SETTER_DEFAULT(BTGraphEditor, behaviour_tree);
}