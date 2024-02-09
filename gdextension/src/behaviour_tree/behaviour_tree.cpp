#include "behaviour_tree.hpp"
#include <godot_cpp/variant/utility_functions.hpp>


godot::Dictionary BehaviourTree::get_task_map()
{ 
    godot::Dictionary dict;
    for (const godot::KeyValue<int, godot::Ref<BTTask>>& element : task_map)
    {
        dict[element.key] = element.value;
    }
    return dict;
}
void BehaviourTree::set_task_map(godot::Dictionary dict)
{
    this->task_map.clear();
    godot::Array keys = dict.keys();
    for (int i = 0, size = keys.size(); i < size; i++)
    {
        this->task_map.insert(keys[i], dict[keys[i]]);
    }
}
void BehaviourTree::set_description(const godot::String &description)
{
    this->description = description;
}

void BehaviourTree::set_root_task(godot::Ref<BTTask> task)
{
    this->root_task = task;
}

int BehaviourTree::get_valid_id() const
{
    return (this->task_map.size() > 0) ? (this->task_map.back()->key() + 1) : 1;
}

int BehaviourTree::get_task_id(godot::Ref<BTTask> task) const
{
    for (const godot::KeyValue<int, godot::Ref<BTTask>> &key_value : this->task_map)
    {
        if (key_value.value == task)
        {
            return key_value.key;
        }
    }
    return -1;
}

godot::Ref<BTTask> BehaviourTree::get_task(int id) const
{
    if (this->task_map.find(id) == nullptr)
    {
        return godot::Ref<BTTask>(nullptr);
    }
    return this->task_map[id];
}


bool BehaviourTree::has_task(godot::Ref<BTTask> task) const
{
    return (this->get_task_id(task) != -1);
}

void BehaviourTree::add_task(int id, godot::Ref<BTTask> task)
{
    ERR_FAIL_COND(task_map.has(id));
    this->task_map.insert(id, task);

    if (!(this->root_task.is_valid()))
    {
        this->set_root_task(task);
    }
}

void BehaviourTree::remove_task_by_ref(godot::Ref<BTTask> task)
{
    /* TODO: 
     * Detach parent, children.
     * */

    int id = this->get_task_id(task);
    this->task_map.erase(id);
    if (this->root_task == task)
    {
        this->root_task.unref();
        if (task_map.size() > 0)
        {
            godot::Ref<BTTask> some_task = this->task_map.front()->get();
            this->root_task = some_task->get_root();
        }
    }
}

void BehaviourTree::remove_task(int id)
{
    /* TODO: 
     * Detach parent, children.
     * */

    godot::Ref<BTTask> task = task_map[id];
    this->task_map.erase(id);
    if (this->root_task == task)
    {
        this->root_task.unref();
        if (task_map.size() > 0)
        {
            godot::Ref<BTTask> some_task = this->task_map.front()->get();
            this->root_task = some_task->get_root();
        }
    }
}

void BehaviourTree::clear_tasks()
{
    godot::UtilityFunctions::print("Clearing.");
    this->task_map.clear();
    this->root_task.unref();
}

godot::Array BehaviourTree::get_tasks() const
{
    godot::Array array;
    int size = this->task_map.size();
    array.resize(size);
    int i = 0;
    for (const godot::KeyValue<int, godot::Ref<BTTask>> &key_value : this->task_map)
    {
        array[i] = key_value.value;
        i++;
    }
    return array;
}

void BehaviourTree::connect_tasks(godot::Ref<BTTask> parent, godot::Ref<BTTask> child, int child_pos)
{
    ERR_FAIL_COND_MSG(!can_connect(parent, child), "Cannot connect parent to child.");
    parent->add_child_at_index(child, child_pos);
    if (child == this->root_task)
    {
        this->root_task = parent;
    }
}

void BehaviourTree::disconnect_tasks(godot::Ref<BTTask> parent, godot::Ref<BTTask> child)
{
    ERR_FAIL_COND_MSG(!(parent->has_child(child)), "Cannot remove child from parent.");
    parent->remove_child(child);
}


void BehaviourTree::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("add_task", "task"), &BehaviourTree::add_task);
    ClassDB::bind_method(D_METHOD("remove_task", "id"), &BehaviourTree::remove_task);
    ClassDB::bind_method(D_METHOD("remove_task_by_ref", "task"), &BehaviourTree::remove_task_by_ref);
    ClassDB::bind_method(D_METHOD("clear_tasks"), &BehaviourTree::clear_tasks);
    ClassDB::bind_method(D_METHOD("connect_tasks", "parent", "child", "child_pos"), &BehaviourTree::connect_tasks);
    ClassDB::bind_method(D_METHOD("disconnect_tasks", "parent", "child"), &BehaviourTree::disconnect_tasks);

    ClassDB::bind_method(D_METHOD("set_description", "description"), &BehaviourTree::set_description);
    ClassDB::bind_method(D_METHOD("get_description"), &BehaviourTree::get_description);
    ClassDB::bind_method(D_METHOD("set_root_task", "task"), &BehaviourTree::set_root_task);
    ClassDB::bind_method(D_METHOD("get_root_task"), &BehaviourTree::get_root_task);
    ClassDB::bind_method(D_METHOD("get_all_tasks"), &BehaviourTree::get_tasks);
    ClassDB::bind_method(D_METHOD("set_task_map", "task_map"), &BehaviourTree::set_task_map);
    ClassDB::bind_method(D_METHOD("get_task_map"), &BehaviourTree::get_task_map);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
    /* TODO: show or no show? , PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR */
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root_task"), "set_root_task", "get_root_task");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "task_map"), "set_task_map", "get_task_map");
}
