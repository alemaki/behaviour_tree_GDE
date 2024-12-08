#include "bt_editor_debugger_plugin.hpp"

bool BTEditorDebuggerPlugin::register_tree_root(godot::Ref<godot::EncodedObjectAsID> node_id)
{
    ERR_FAIL_COND_V(this->last_registered_tree_name.is_empty(), false);

    tree_to_root_id.insert(last_registered_tree_name, node_id);
    
    return true;
}

bool BTEditorDebuggerPlugin::register_tree(const godot::Array& p_data)
{
    ERR_FAIL_COND_V(p_data.size() != 1, false);
    
    this->last_registered_tree_name = p_data[0];

    return true;
}

bool BTEditorDebuggerPlugin::register_task(const godot::Array& p_data)
{
    ERR_FAIL_COND_V(p_data.size() != 5, false);
    godot::Ref<godot::EncodedObjectAsID> node_id = p_data[0];

    TaskInfo new_task;
    new_task.name = p_data[1];
    new_task.custom_name = p_data[2];
    new_task.class_name = p_data[3];
    new_task.parent = p_data[4];

    if (this->tasks.has(new_task.parent))
    {
        TaskInfo& parent_info = this->tasks.get(new_task.parent);
        parent_info.children.push_back(node_id);
        new_task.tree_name = parent_info.tree_name;
    }
    else
    {
        this->register_tree_root(node_id);
        new_task.tree_name = this->last_registered_tree_name;
        this->last_registered_tree_name = "";
    }

    tasks.insert(node_id, new_task);

    return true;
}

bool BTEditorDebuggerPlugin::debug_tree(const godot::Array &p_data, int32_t p_session_id)
{
    ERR_FAIL_COND_V(p_data.size() != 1, false);
    
    godot::Ref<godot::EditorDebuggerSession> session = this->get_session(p_session_id);
    ERR_FAIL_COND_V(session.is_null(), false);
    godot::StringName tree_name = p_data[0];
    godot::UtilityFunctions::print_rich("Debugging " + tree_name + ".");

    ERR_FAIL_COND_V(!this->tree_to_root_id.has(tree_name), false);

    BTGraphView* graph_view = memnew(BTGraphView);
    this->tree_tab_container->add_child(memnew(BTGraphView));

    const TaskInfo& root = this->tasks[this->tree_to_root_id[tree_name]];
    godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_child_names;
    godot::Vector<godot::Ref<EncodedObjectAsID>> tasks_to_create;
    tasks_to_create.push_back(this->tree_to_root_id[tree_name]);
    int current_task_index = 0;

    while (current_task_index < tasks_to_create.size())
    {
        const TaskInfo& task_info = this->tasks[tasks_to_create[current_task_index]];
        current_task_index++;
        graph_view->create_task_node(task_info.name, task_info.class_name);
        graph_view->set_task_node_title(task_info.name, task_info.custom_name);
        if (task_info.children.size() != 0)
        {
            parent_to_child_names[task_info.name] = {};
        }
        for (const godot::Ref<godot::EncodedObjectAsID> child_id : task_info.children)
        {
            tasks_to_create.push_back(child_id);
            godot::UtilityFunctions::print(child_id);
            parent_to_child_names[task_info.name].push_back(this->tasks[child_id].name);
        }
    }

    for (godot::Ref<EncodedObjectAsID> task_id : tasks_to_create)
    {
        const TaskInfo& task_info = this->tasks[task_id];
        if (task_info.parent.is_valid())
        {
            const TaskInfo& parent_info = this->tasks[task_info.parent];
            graph_view->connect_task_nodes(parent_info.name, task_info.name);
        }
    }

    godot::UtilityFunctions::print_rich(root.name);
    godot::UtilityFunctions::print_rich(graph_view->get_child_count());
    graph_view->arrange_nodes(root.name, parent_to_child_names);

    return true;
}

bool BTEditorDebuggerPlugin::_has_capture(const String &p_capture) const
{
    return p_capture == "bt_debug";
}

bool BTEditorDebuggerPlugin::_capture(const String &p_message, const godot::Array &p_data, int32_t p_session_id)
{
    if (p_message == "bt_debug:register_tree")
    {
        return this->register_tree(p_data);
    }
    else if (p_message == "bt_debug:register_task")
    {
        return this->register_task(p_data);
    }
    else if (p_message == "bt_debug:debug_tree")
    {
        return this->debug_tree(p_data, p_session_id);
    }
    
    return false;
}

void BTEditorDebuggerPlugin::_setup_session(int32_t p_session_id)
{
    if (this->tree_tab_container)
    {
        memdelete(this->tree_tab_container);
    }
    this->tree_tab_container = memnew(godot::TabContainer);

    godot::Ref<godot::EditorDebuggerSession> session = get_session(p_session_id);

    ERR_FAIL_COND(session.is_null());
    session->add_session_tab(tree_tab_container);
    session->call_deferred("connect", "started", callable_mp(this, &BTEditorDebuggerPlugin::on_session_started));
    session->call_deferred("connect", "stopped", callable_mp(this, &BTEditorDebuggerPlugin::on_session_stopped));
}

void BTEditorDebuggerPlugin::on_session_started()
{
    godot::UtilityFunctions::print("Debugger session started");
}

void BTEditorDebuggerPlugin::on_session_stopped()
{
    godot::UtilityFunctions::print("Debugger session stopped");
}