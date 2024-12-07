#include "bt_editor_debugger_plugin.hpp"

bool BTEditorDebuggerPlugin::register_tree_root(godot::Ref<godot::EncodedObjectAsID> node_id, int32_t p_session_id)
{
    ERR_FAIL_COND_V(this->last_registered_tree_name.is_empty(), false);
    
    godot::Ref<godot::EditorDebuggerSession> session = this->get_session(p_session_id);
    ERR_FAIL_COND_V(session.is_null(), false);

    tree_to_root_id.insert(last_registered_tree_name, node_id);
    
    return true;
}

bool BTEditorDebuggerPlugin::register_tree(const Array& p_data, int32_t p_session_id)
{
    ERR_FAIL_COND_V(p_data.size() < 1, false);
    
    this->last_registered_tree_name = p_data[0];

    return true;
}

bool BTEditorDebuggerPlugin::register_task(const Array& p_data, int32_t p_session_id)
{
    ERR_FAIL_COND_V(p_data.size() < 1, false);

    for (int i = 0, size = p_data.size(); i < size; i++)
    {
        godot::UtilityFunctions::print(p_data[i]);
    }

    godot::Ref<godot::EncodedObjectAsID> node_id = p_data[0];

    NodeInfo new_task;
    new_task.name = p_data[1];
    new_task.custom_name = p_data[2];
    new_task.class_name = p_data[3];
    new_task.parent = p_data[4];

    if (this->tasks.has(new_task.parent))
    {
        NodeInfo& parent_info = this->tasks.get(new_task.parent);
        parent_info.children.push_back(node_id);
    }
    else
    {
        this->register_tree_root(node_id, p_session_id);
        this->last_registered_tree_name = "";
    }

    tasks.insert(node_id, new_task);

    return true;
}

bool BTEditorDebuggerPlugin::_has_capture(const String &p_capture) const
{
    return p_capture == "bt_debug";
}

bool BTEditorDebuggerPlugin::_capture(const String &p_message, const Array &p_data, int32_t p_session_id)
{
    if (p_message == "bt_debug:register_tree")
    {
        return this->register_tree(p_data, p_session_id);
    }
    else if (p_message == "bt_debug:register_task")
    {
        return this->register_task(p_data, p_session_id);
    }
    
    return false;
}

void BTEditorDebuggerPlugin::_setup_session(int32_t p_session_id)
{
    godot::Label *label = memnew(godot::Label);
    label->set_text("BT Debugger Initialized");
    label->set_name("BT Debugger");

    godot::Ref<godot::EditorDebuggerSession> session = get_session(p_session_id);

    ERR_FAIL_COND(session.is_null());
    session->add_session_tab(label);
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
