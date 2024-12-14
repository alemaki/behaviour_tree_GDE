#ifndef BT_EDITOR_DEBUGGER_MESSAGES_HPP
#define BT_EDITOR_DEBUGGER_MESSAGES_HPP

#include <godot_cpp/classes/engine_debugger.hpp>

class BTEdtiorDebuggerMessages
{
private:
    static void send_message_register_tree(godot::StringName tree_name, godot::Variant _root);
    static void send_message_register_tree_task(godot::Variant _task);
    static void register_root_task(godot::StringName tree_name, godot::Variant _root);

public:
    static bool can_send_message();
    static void register_tree(godot::StringName tree_name, godot::Variant _root);
    static void debug_tree(godot::StringName tree_name);
    static void task_status_change(godot::Variant _root, godot::Variant _status);
};

#endif /* BT_EDITOR_DEBUGGER_MESSAGES_HPP */