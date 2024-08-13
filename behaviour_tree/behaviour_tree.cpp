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

void BehaviourTree::set_root_task(godot::Ref<BTTask> task)
{
    ERR_FAIL_COND_MSG(!(this->has_task(task)), "Task not found.");
    this->root_task = task;
}

void BehaviourTree::set_custom_name_task_by_ref(godot::Ref<BTTask> task, const godot::String& new_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task(task)), "Task not found.");
    task->set_custom_name(new_name);
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

void BehaviourTree::add_task_by_ref(godot::Ref<BTTask> task)
{
    this->add_task(this->get_valid_id(), task);
}
void BehaviourTree::add_task(int id, godot::Ref<BTTask> task)
{
    ERR_FAIL_COND_MSG(this->task_map.has(id), "ID already added.");
    ERR_FAIL_COND_MSG(this->has_task(task), "Task already added.");

    this->task_map.insert(id, task);

    if (!(this->root_task.is_valid()))
    {
        this->set_root_task(task);
    }
}

void BehaviourTree::detach_task_by_ref(godot::Ref<BTTask> task)
{
    ERR_FAIL_COND_MSG(!(this->has_task(task)), "Task is not in tree.");

    int id = this->get_task_id(task);
    this->detach_task(id);
}

void BehaviourTree::detach_task(int id)
{
    ERR_FAIL_COND_MSG(!(this->task_map.has(id)), "Task id is not in tree.");

    godot::Ref<BTTask> task = this->task_map[id];
    if (task->get_parent().is_valid())
    {
        task->get_parent()->remove_child(task);
    }

    task->set_children({});
}

void BehaviourTree::remove_task_by_ref(godot::Ref<BTTask> task)
{
    int id = this->get_task_id(task);
    this->remove_task(id);
}

void BehaviourTree::remove_task(int id)
{
    godot::Ref<BTTask> task = this->task_map[id];
    this->detach_task(id);

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
    // TODO: if child is root, change root maybe?
}

void BehaviourTree::disconnect_tasks(godot::Ref<BTTask> parent, godot::Ref<BTTask> child)
{
    ERR_FAIL_COND_MSG(!(parent->has_child(child)), "Cannot remove child from parent.");
    parent->remove_child(child);
}

void BehaviourTree::set_tasks_of_parent(godot::Ref<BTTask> parent, godot::Array new_children)
{
    ERR_FAIL_COND_MSG(!(this->has_task(parent)), "Parent not in tree.");

    for (int i = 0 , size = new_children.size(); i < size; i++)
    {
        godot::Ref<BTTask> task = godot::Ref<BTTask>(new_children[i]);
        ERR_FAIL_COND_MSG(!(this->has_task(task)), "Child not in tree.");
    }
    parent->set_children(new_children);
}

void BehaviourTree::swap_task_in(godot::Ref<BTTask> old_task, godot::Ref<BTTask> new_task)
{
    int id = this->get_task_id(old_task);
    ERR_FAIL_COND_MSG(id == -1, "Old task not in tree.");

    if (old_task->get_parent().is_valid())
    {
        godot::Ref<BTTask> parent = old_task->get_parent();
        parent->swap_child(old_task, new_task);
    }
    else
    {
        godot::Array children = old_task->get_children();
        old_task->set_children(godot::Array());
        new_task->set_children(children);
    }

    if (this->root_task == old_task)
    {
        this->root_task = new_task;
    }

    this->remove_task_by_ref(old_task);
    this->add_task(id, new_task);
}

godot::Ref<BTTask> BehaviourTree::instantiate(Node* actor, godot::Ref<Blackboard> blackboard) const
{
    if (root_task.is_null())
    {
        godot::UtilityFunctions::printerr("Behaviour tree is empty.");
        return nullptr;
    }
    godot::Ref<BTTask> task = this->root_task->clone();
    task->initialize(actor, blackboard);
    return task;
}

void BehaviourTree::_bind_methods()
{
    using namespace godot;
    
    ClassDB::bind_method(D_METHOD("get_valid_id"), &BehaviourTree::get_valid_id);
    ClassDB::bind_method(D_METHOD("get_task_id", "task"), &BehaviourTree::get_task_id);
    ClassDB::bind_method(D_METHOD("get_task", "id"), &BehaviourTree::get_task);
    ClassDB::bind_method(D_METHOD("has_task", "task"), &BehaviourTree::has_task);
    ClassDB::bind_method(D_METHOD("add_task_by_ref", "task"), &BehaviourTree::add_task_by_ref);
    ClassDB::bind_method(D_METHOD("add_task", "id", "task"), &BehaviourTree::add_task);
    ClassDB::bind_method(D_METHOD("detach_task_by_ref", "task"), &BehaviourTree::detach_task_by_ref);
    ClassDB::bind_method(D_METHOD("detach_task", "id"), &BehaviourTree::detach_task);
    ClassDB::bind_method(D_METHOD("remove_task_by_ref", "task"), &BehaviourTree::remove_task_by_ref);
    ClassDB::bind_method(D_METHOD("remove_task", "id"), &BehaviourTree::remove_task);
    ClassDB::bind_method(D_METHOD("clear_tasks"), &BehaviourTree::clear_tasks);
    ClassDB::bind_method(D_METHOD("get_tasks"), &BehaviourTree::get_tasks);
    ClassDB::bind_method(D_METHOD("connect_tasks", "parent", "child", "child_pos"), &BehaviourTree::connect_tasks);
    ClassDB::bind_method(D_METHOD("disconnect_tasks", "parent", "child"), &BehaviourTree::disconnect_tasks);
    ClassDB::bind_method(D_METHOD("set_tasks_of_parent", "parent", "new_children"), &BehaviourTree::set_tasks_of_parent);
    ClassDB::bind_method(D_METHOD("set_custom_name_task_by_ref", "task", "task_name"), &BehaviourTree::set_custom_name_task_by_ref);
    ClassDB::bind_method(D_METHOD("swap_task_in", "old_task", "new_task"), &BehaviourTree::swap_task_in);
    ClassDB::bind_method(D_METHOD("instantiate", "actor", "blackboard"), &BehaviourTree::instantiate);

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BehaviourTree, STRING, description);
    BIND_GETTER_SETTER(BehaviourTree, root_task, task);
    BIND_GETTER_SETTER_DEFAULT(BehaviourTree, task_map);

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "task_map", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_task_map", "get_task_map");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root_task", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_root_task", "get_root_task");
}
