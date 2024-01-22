#include "behaviour_tree.hpp"

void BehaviorTree::set_description(const godot::String &description)
{
    this->description = description;
}
godot::String BehaviorTree::get_description() const
{
    return this->description;
}

void BehaviorTree::set_root_task(godot::Ref<BTTask> task)
{
    this->root_task = task;
}
godot::Ref<BTTask> BehaviorTree::get_root_task() const
{
    return this->root_task;
}
void BehaviorTree::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_description", "description"), &BehaviorTree::set_description);
    ClassDB::bind_method(D_METHOD("get_description"), &BehaviorTree::get_description);
    ClassDB::bind_method(D_METHOD("set_root_task", "task"), &BehaviorTree::set_root_task);
    ClassDB::bind_method(D_METHOD("get_root_task"), &BehaviorTree::get_root_task);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root_task"), "set_root_task", "get_root_task");
}
