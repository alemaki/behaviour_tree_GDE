#ifndef BT_EDITOR_DEBUGGER_PLUGIN_HPP
#define BT_EDITOR_DEBUGGER_PLUGIN_HPP

#include <godot_cpp/classes/editor_debugger_plugin.hpp>
#include <godot_cpp/classes/editor_debugger_session.hpp>
#include <godot_cpp/classes/tab_container.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_set.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/ui/bt_graph_view.hpp"

class BTEditorDebuggerPlugin : public godot::EditorDebuggerPlugin
{
    GDCLASS(BTEditorDebuggerPlugin, godot::EditorDebuggerPlugin);

    struct TaskInfo
    {
        BTTask::Status status = BTTask::Status::FRESH;
        godot::StringName tree_name;
        godot::StringName name;
        godot::StringName custom_name;
        godot::StringName class_name;
        godot::StringName parent;
        godot::Vector<godot::StringName> children;
    };

private:
    godot::TabContainer* tree_tab_container = nullptr;
    godot::HashMap<godot::StringName, int32_t> tree_to_tab_index;
    godot::HashMap<godot::StringName, godot::StringName> tree_to_root_name;
    godot::HashMap<godot::StringName, TaskInfo> task_name_to_info;

private:
    void register_tree(const Array& p_data);
    void register_task(const Array& p_data);
    void debug_tree(const Array& p_data, int32_t p_session_id);

public:
    ~BTEditorDebuggerPlugin();
    virtual bool _has_capture(const String &p_capture) const override;
    virtual bool _capture(const String &p_message, const Array &p_data, int32_t p_session_id) override;
    virtual void _setup_session(int32_t p_session_id) override;

    void on_session_started();
    void on_session_stopped();

protected:
    static void _bind_methods(){};
};

#endif /* BT_EDITOR_DEBUGGER_PLUGIN_HPP */