#include "bt_editor_debugger_messages.hpp"
#include <godot_cpp/classes/engine.hpp>

bool BTEdtiorDebuggerMessages::can_send_message()
{
    return !(godot::Engine::get_singleton()->is_editor_hint());
}

void BTEdtiorDebuggerMessages::send_message_register_tree(godot::StringName tree_name)
{
    godot::Array arr;
    arr.push_back(tree_name);
    godot::EngineDebugger::get_singleton()->send_message("bt_debug:register_tree", arr);
}

void BTEdtiorDebuggerMessages::register_tree(godot::StringName tree_name)
{
    ERR_FAIL_COND(tree_name.is_empty());
    if (!(BTEdtiorDebuggerMessages::can_send_message()))
    {
        return;
    }
    BTEdtiorDebuggerMessages::send_message_register_tree(tree_name);
}

void BTEdtiorDebuggerMessages::send_message_register_tree_task(godot::Ref<BTTask> task)
{
    godot::Array arr;
    arr.push_back(task);
    arr.push_back(task->get_name());
    arr.push_back(task->get_custom_name());
    arr.push_back(task->get_class());
    arr.push_back(task->get_parent());
    godot::EngineDebugger::get_singleton()->send_message("bt_debug:register_task", arr);
}

void BTEdtiorDebuggerMessages::register_tree_task(godot::Ref<BTTask> root)
{
    ERR_FAIL_COND(root.is_null());
    if (!(BTEdtiorDebuggerMessages::can_send_message()))
    {
        return;
    }
    godot::Vector<godot::Ref<BTTask>> tasks;
    tasks.push_back(root);
    int current_task_index = 0;

    while (tasks.size() > current_task_index)
    {
        godot::Ref<BTTask> task = tasks[current_task_index];
        current_task_index++;
         BTEdtiorDebuggerMessages::send_message_register_tree_task(task);

        godot::Array children = task->get_children();
        for (int j = 0, size = children.size(); j < size; j++)
        {
            tasks.push_back(godot::Ref<BTTask>(children[j]));
        }
    }
}