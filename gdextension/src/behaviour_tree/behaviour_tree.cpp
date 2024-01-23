#include "behaviour_tree.hpp"

void BehaviourTree::set_description(const godot::String &description)
{
    this->description = description;
}

void BehaviourTree::set_root_task(godot::Ref<BTTask> task)
{
    this->root_task = task;
}
void BehaviourTree::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_description", "description"), &BehaviourTree::set_description);
    ClassDB::bind_method(D_METHOD("get_description"), &BehaviourTree::get_description);
    ClassDB::bind_method(D_METHOD("set_root_task", "task"), &BehaviourTree::set_root_task);
    ClassDB::bind_method(D_METHOD("get_root_task"), &BehaviourTree::get_root_task);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root_task"), "set_root_task", "get_root_task");
}
