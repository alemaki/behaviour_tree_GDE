#include "behaviour_tree.hpp"

void BehaviourTree::set_description(const godot::String &description)
{
    this->description = description;
}

void BehaviourTree::set_root_task(godot::Ref<BTTask> task)
{
    this->root_task = task;
}

void BehaviourTree::add_task(godot::Ref<BTTask> task)
{
    this->all_tasks.push_back(task);
    if (!(this->root_task.is_valid()))
    {
        this->root_task = task;
    }
}

void BehaviourTree::remove_task(godot::Ref<BTTask> task)
{
    /* TODO: 
     * Detach parent, children.
     * Check if root task removed.
     * Reorder all tasks.
     * */ 
}
void BehaviourTree::clear_tasks()
{
    this->all_tasks.clear();
    this->root_task.unref();
}
void BehaviourTree::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_description", "description"), &BehaviourTree::set_description);
    ClassDB::bind_method(D_METHOD("get_description"), &BehaviourTree::get_description);
    ClassDB::bind_method(D_METHOD("set_root_task", "task"), &BehaviourTree::set_root_task);
    ClassDB::bind_method(D_METHOD("get_root_task"), &BehaviourTree::get_root_task);
    ClassDB::bind_method(D_METHOD("add_task", "task"), &BehaviourTree::add_task);
    ClassDB::bind_method(D_METHOD("remove_task", "task"), &BehaviourTree::remove_task);
    ClassDB::bind_method(D_METHOD("clear_tasks"), &BehaviourTree::clear_tasks);
    ClassDB::bind_method(D_METHOD("get_all_tasks"), &BehaviourTree::get_all_tasks);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root_task"), "set_root_task", "get_root_task");
}
