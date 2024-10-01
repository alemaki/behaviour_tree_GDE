#ifndef BT_EDITOR_DEBUGGER_MESSAGES_HPP
#define BT_EDITOR_DEBUGGER_MESSAGES_HPP

#include <godot_cpp/classes/engine_debugger.hpp>
#include "behaviour_tree/tasks/bt_task.hpp"

class BTEdtiorDebuggerMessages
{
public:
    static bool can_send_message();
    static void register_tree(godot::Ref<BTTask> root);
};

#endif /* BT_EDITOR_DEBUGGER_MESSAGES_HPP */