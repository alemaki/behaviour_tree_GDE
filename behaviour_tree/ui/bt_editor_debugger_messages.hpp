#ifndef BT_EDITOR_DEBUGGER_MESSAGES_HPP
#define BT_EDITOR_DEBUGGER_MESSAGES_HPP

#include <godot_cpp/classes/engine_debugger.hpp>
#include "behaviour_tree/tasks/bt_task.hpp"

class BTEdtiorDebuggerMessages
{
private:
    static void send_message_register_tree(godot::StringName tree_name);
    static void send_message_register_tree_task(godot::Ref<BTTask> root);

public:
    static bool can_send_message();
    static void register_tree(godot::StringName tree_name);
    static void register_tree_task(godot::Ref<BTTask> root);
};

#endif /* BT_EDITOR_DEBUGGER_MESSAGES_HPP */