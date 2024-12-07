#ifndef BT_EDITOR_DEBUGGER_PLUGIN_HPP
#define BT_EDITOR_DEBUGGER_PLUGIN_HPP

#include <godot_cpp/classes/editor_debugger_plugin.hpp>
#include <godot_cpp/classes/editor_debugger_session.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/encoded_object_as_id.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_set.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"

class BTEditorDebuggerPlugin : public godot::EditorDebuggerPlugin
{
    GDCLASS(BTEditorDebuggerPlugin, godot::EditorDebuggerPlugin);

    struct NodeInfo
    {
        BTTask::Status status = BTTask::Status::FRESH;
        godot::StringName name;
        godot::StringName custom_name;
        godot::StringName class_name;
        godot::Ref<godot::EncodedObjectAsID> parent;
        godot::Vector<godot::Ref<godot::EncodedObjectAsID>> children;
    };

private:
    godot::StringName last_registered_tree_name;
    godot::HashMap<godot::StringName, godot::Ref<godot::EncodedObjectAsID>> tree_to_root_id;
    godot::HashMap<godot::Ref<godot::EncodedObjectAsID>, NodeInfo> tasks;

private:
    bool register_tree_root(godot::Ref<godot::EncodedObjectAsID> node_id, int32_t p_session_id);
    bool register_tree(const Array& p_data, int32_t p_session_id);
    bool register_task(const Array& p_data, int32_t p_session_id);

public:
    virtual bool _has_capture(const String &p_capture) const override;
    virtual bool _capture(const String &p_message, const Array &p_data, int32_t p_session_id) override;
    virtual void _setup_session(int32_t p_session_id) override;

    void on_session_started();
    void on_session_stopped();

protected:
    static void _bind_methods(){};
};

#endif /* BT_EDITOR_DEBUGGER_PLUGIN_HPP */