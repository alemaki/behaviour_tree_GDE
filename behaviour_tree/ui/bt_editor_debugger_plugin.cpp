#include "bt_editor_debugger_plugin.hpp"

BTEditorDebuggerPlugin::~BTEditorDebuggerPlugin()
{
    if (this->tree_tab_container)
    {
        memdelete(this->tree_tab_container);
    }
}

void BTEditorDebuggerPlugin::register_tree(const godot::Array& p_data)
{
    ERR_FAIL_COND(p_data.size() != 2);
    
    godot::StringName tree_name = p_data[0];
    godot::StringName task_name = p_data[1];

    this->tree_to_root_name.insert(tree_name, task_name);
}

void BTEditorDebuggerPlugin::register_task(const godot::Array& p_data)
{
    ERR_FAIL_COND(p_data.size() != 4);
    godot::StringName node_name = p_data[0];

    TaskInfo new_task;
    new_task.name = node_name;
    new_task.custom_name = p_data[1];
    new_task.class_name = p_data[2];
    new_task.parent = p_data[3];

    if (this->task_name_to_info.has(new_task.parent))
    {
        TaskInfo& parent_info = this->task_name_to_info.get(new_task.parent);
        parent_info.children.push_back(node_name);
        new_task.tree_name = parent_info.tree_name;
    }
    else
    {
        godot::StringName tree_name;
        for (const godot::KeyValue<godot::StringName, godot::StringName> key_value : this->tree_to_root_name)
        {
            if (key_value.value == node_name)
            {
                tree_name = key_value.key;
                break;
            }
        }
        ERR_FAIL_COND(tree_name.is_empty());
        new_task.tree_name = tree_name;
    }

    this->task_name_to_info.insert(node_name, new_task);
}

void BTEditorDebuggerPlugin::debug_tree(const godot::Array &p_data, int32_t p_session_id)
{
    ERR_FAIL_COND(p_data.size() != 1);
    
    godot::Ref<godot::EditorDebuggerSession> session = this->get_session(p_session_id);
    ERR_FAIL_COND(session.is_null());
    godot::StringName tree_name = p_data[0];

    ERR_FAIL_COND(!this->tree_to_root_name.has(tree_name));

    BTGraphView* graph_view = memnew(BTGraphView);
    this->tree_tab_container->add_child(graph_view);

    const TaskInfo& root = this->task_name_to_info[this->tree_to_root_name[tree_name]];
    godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_child_names;
    godot::Vector<godot::StringName> tasks_to_create;
    tasks_to_create.push_back(this->tree_to_root_name[tree_name]);
    int current_task_index = 0;

    while (current_task_index < tasks_to_create.size())
    {
        const TaskInfo& task_info = this->task_name_to_info[tasks_to_create[current_task_index]];
        current_task_index++;
        graph_view->create_task_node(task_info.name, task_info.class_name);
        graph_view->set_task_node_title(task_info.name, task_info.custom_name);
        for (const godot::StringName& child_name : task_info.children)
        {
            tasks_to_create.push_back(child_name);
        }
    }

    for (const godot::StringName& task_name : tasks_to_create)
    {
        const TaskInfo& task_info = this->task_name_to_info[task_name];
        if (!task_info.parent.is_empty())
        {
            graph_view->connect_task_nodes(task_info.parent, task_name);
            parent_to_child_names[task_info.parent].push_back(task_name);
        }
    }

    graph_view->arrange_nodes(root.name, parent_to_child_names);
    graph_view->queue_redraw();
}

bool BTEditorDebuggerPlugin::_has_capture(const String &p_capture) const
{
    return p_capture == "bt_debug";
}

bool BTEditorDebuggerPlugin::_capture(const String &p_message, const godot::Array &p_data, int32_t p_session_id)
{
    if (p_message == "bt_debug:register_tree")
    {
        this->register_tree(p_data);
        return true;
    }
    else if (p_message == "bt_debug:register_task")
    {
        this->register_task(p_data);
        return true;
    }
    else if (p_message == "bt_debug:debug_tree")
    {
        this->debug_tree(p_data, p_session_id);
        return true;
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