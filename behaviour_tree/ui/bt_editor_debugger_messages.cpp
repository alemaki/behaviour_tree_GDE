#include "bt_editor_debugger_messages.hpp"
#include <godot_cpp/classes/engine.hpp>

bool BTEdtiorDebuggerMessages::can_send_message()
{
    return !(godot::Engine::get_singleton()->is_editor_hint());
}

void BTEdtiorDebuggerMessages::register_tree(godot::Ref<BTTask> root)
{
    if (!(BTEdtiorDebuggerMessages::can_send_message()))
    {
        return;
    }
    godot::Array arr;
    arr.push_back(root);
    godot::EngineDebugger::get_singleton()->send_message("bt_debug:register_tree", arr);
}