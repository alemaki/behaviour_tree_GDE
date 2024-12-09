#include "bt_editor_debugger_messages.hpp"
#include <godot_cpp/classes/engine.hpp>

bool BTEdtiorDebuggerMessages::can_send_message()
{
    return !(godot::Engine::get_singleton()->is_editor_hint());
}

void BTEdtiorDebuggerMessages::send_message_register_tree(godot::StringName tree_name, godot::Ref<BTTask> root)
{
    godot::EngineDebugger::get_singleton()->send_message("bt_debug:register_tree", godot::Array::make(tree_name, root->get_name()));
}

void BTEdtiorDebuggerMessages::register_tree(godot::StringName tree_name, godot::Ref<BTTask> root)
{
    ERR_FAIL_COND(tree_name.is_empty());
    ERR_FAIL_COND(root.is_null());
    if (!(BTEdtiorDebuggerMessages::can_send_message()))
    {
        return;
    }
    root->set_default_name();
    BTEdtiorDebuggerMessages::send_message_register_tree(tree_name, root);

    BTEdtiorDebuggerMessages::register_root_task(tree_name, root);
}

void BTEdtiorDebuggerMessages::send_message_register_tree_task(godot::Ref<BTTask> task)
{
    godot::Array arr;
    arr.push_back(task->get_name());
    arr.push_back(task->get_custom_name());
    arr.push_back(task->get_class());
    if (task->get_parent().is_null())
    {
        arr.push_back("");
    }
    else
    {
        arr.push_back(task->get_parent()->get_name());
    }
    godot::EngineDebugger::get_singleton()->send_message("bt_debug:register_task", arr);
}

void BTEdtiorDebuggerMessages::register_root_task(godot::StringName tree_name, godot::Ref<BTTask> root)
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
        task->set_default_name();
        current_task_index++;
        BTEdtiorDebuggerMessages::send_message_register_tree_task(task);

        godot::Array children = task->get_children();
        for (int j = 0, size = children.size(); j < size; j++)
        {
            tasks.push_back(godot::Ref<BTTask>(children[j]));
        }
    }
}

void  BTEdtiorDebuggerMessages::debug_tree(godot::StringName tree_name)
{
    if (!(BTEdtiorDebuggerMessages::can_send_message()))
    {
        return;
    }
    godot::EngineDebugger::get_singleton()->send_message("bt_debug:debug_tree", godot::Array::make(tree_name));
}