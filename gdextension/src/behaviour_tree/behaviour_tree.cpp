#include "behaviour_tree.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
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
     * */

    this->all_tasks.erase(task);
    if (this->root_task == task)
    {
        root_task.unref();
        if (all_tasks.size() > 0)
        {
            root_task = all_tasks[0]->get_root();
        }
    }
}
void BehaviourTree::clear_tasks()
{
    this->all_tasks.clear();
    this->root_task.unref();
}


void BehaviourTree::set_tasks(godot::Array all_tasks)
{
    int size = all_tasks.size();
    int invalid_refs = 0;

    this->all_tasks.clear();
    this->all_tasks.resize(size);

    for (int index = 0; index < size; index++)
    {
        godot::Ref<BTTask> child = all_tasks[index];
        if (child.is_null())
        {
            // TODO: error
            invalid_refs++;
            continue;
        }
        this->all_tasks.set(index - invalid_refs, child);
    }

    if (invalid_refs != 0)
    {
        this->all_tasks.resize(size - invalid_refs);
    }
}

godot::Array BehaviourTree::get_tasks() const
{
    godot::Array array;
    int size = this->all_tasks.size();
    array.resize(size);
    for (int i = 0; i < size; i++)
    {
        array[i] = this->all_tasks[i].ptr();
    }
    return array;
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
    ClassDB::bind_method(D_METHOD("set_all_tasks"), &BehaviourTree::set_tasks);
    ClassDB::bind_method(D_METHOD("get_all_tasks"), &BehaviourTree::get_tasks);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
    /* TODO: show or no show? , PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR */
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root_task"), "set_root_task", "get_root_task");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "all_tasks"), "set_all_tasks", "get_all_tasks"); 
}
