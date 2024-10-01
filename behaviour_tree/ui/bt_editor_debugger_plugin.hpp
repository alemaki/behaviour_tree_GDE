#ifndef BT_EDITOR_DEBUGGER_PLUGIN_HPP
#define BT_EDITOR_DEBUGGER_PLUGIN_HPP

#include <godot_cpp/classes/editor_debugger_plugin.hpp>
#include <godot_cpp/classes/editor_debugger_session.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/templates/vector.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"

class BTEditorDebuggerPlugin : public godot::EditorDebuggerPlugin
{
    GDCLASS(BTEditorDebuggerPlugin, godot::EditorDebuggerPlugin);

private:
    godot::Vector<godot::Ref<BTTask>> trees;

public:

    virtual bool _has_capture(const String &p_capture) const override
    {
        return p_capture == "bt_debug";
    }

    virtual bool _capture(const String &p_message, const Array &p_data, int32_t p_session_id) override
    {
        if (p_message == "bt_debug:register_tree")
        {
            ERR_FAIL_COND_V(p_data.size() < 1, false);
            godot::Ref<BTTask> root = p_data[0];

            godot::Ref<godot::EditorDebuggerSession> session = get_session(p_session_id);
            if (session.is_valid())
            {
                godot::UtilityFunctions::printt(session);
                // if (godot::Label *label = Object::cast_to<godot::Label>(session->get_session_tab("BT Debugger")))
                // {
                //     label->set_text("Tree ID: " + String::num_int64(p_data[0].get_instance_id()));
                // }
            }

            return true;
        }
        return false;
    }

    virtual void _setup_session(int32_t p_session_id) override
    {
        godot::Label *label = memnew(godot::Label);
        label->set_text("BT Debugger Initialized");
        label->set_name("BT Debugger"); // This name is used as the tab title in the session UI

        godot::Ref<godot::EditorDebuggerSession> session = get_session(p_session_id);

        if (session.is_valid())
        {
            session->add_session_tab(label);
            session->call_deferred("connect", "started", callable_mp(this, &BTEditorDebuggerPlugin::on_session_started));
            session->call_deferred("connect", "stopped", callable_mp(this, &BTEditorDebuggerPlugin::on_session_stopped));
        }
    }

    void BTEditorDebuggerPlugin::on_session_started()
    {
        godot::UtilityFunctions::print("Debugger session started");
    }

    void BTEditorDebuggerPlugin::on_session_stopped()
    {
        godot::UtilityFunctions::print("Debugger session stopped");
    }

protected:
    static void _bind_methods(){};
};



#endif /* BT_EDITOR_DEBUGGER_PLUGIN_HPP */