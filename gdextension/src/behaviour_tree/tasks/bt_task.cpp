#include "bt_task.hpp"

godot::Node* BTTask::get_actor() const
{
    return this->actor;
}

void BTTask::set_actor(godot::Node* actor)
{
    this->actor = actor;
}
godot::Ref<BTTask> BTTask::get_parent() const
{
    return godot::Ref<BTTask>(this->parent);
}

int BTTask::get_child_count() const
{
    return this->children.size();
}

godot::Array BTTask::get_children() const
{
    godot::Array array;
    int size = this->get_child_count();
    array.resize(size);
    for (int i = 0; i < size; i++)
    {
        array[i] = this->children[i].ptr();
    }
    return array;
}

void BTTask::set_children(godot::Array array)
{
    int size = array.size();
    int invalid_refs = 0;

    this->children.clear();
    this->children.resize(size);

    for (int index = 0; index < size; index++)
    {
        godot::Ref<BTTask> child = array[index];
        if (child.is_null())
        {
            // TODO: error
            invalid_refs++;
            continue;
        }
        if (child->parent != nullptr && child->parent != this)
        {
            // TODO: clone child or just move it?
        }
        child->parent = this;
        this->children.set(index - invalid_refs, child);
    }

    if (invalid_refs != 0)
    {
        this->children.resize(size - invalid_refs);
    }
}

BTTask::Status BTTask::get_status() const
{
    return this->status;
}

bool BTTask::is_root() const
{
    return (this->parent == nullptr);
}

godot::Ref<BTTask> BTTask::get_root() const
{
    const BTTask* root(this);
    while(root->parent == nullptr)
    {
        root = (root->parent);
    }
    return godot::Ref<BTTask>(root);
}

void BTTask::add_child(godot::Ref<BTTask> child)
{
    if (!(child.is_valid()))
    {
        return;
    }
    if (child->parent != nullptr)
    {
        // TODO:: error
        return;
    }
    child->parent = this;
    this->children.push_back(child);
}

void BTTask::add_child_at_index(int index, godot::Ref<BTTask> child)
{
    if (!(child.is_valid()))
    {
        return;
    }
    if (child->parent != nullptr)
    {
        // TODO:: error
        return;
    }
    if (index < 0 || index > this->children.size())
    {
        index = this->children.size();
    }
    child->parent = this;
    this->children.insert(index, child);
}

void BTTask::remove_child(godot::Ref<BTTask> child)
{
    int index = this->children.find(child);
    if (index == -1)
    {
        // TODO:: error
        return;
    }
    child->parent = nullptr;
    this->children.remove_at(index);
}

void BTTask::remove_child_at_index(int index)
{
    if (index < 0 || index > this->children.size())
    {
        //TODO:: error
        return;
    }
    this->children[index]->parent = nullptr;
    this->children.remove_at(index);
}

bool BTTask::has_child(const godot::Ref<BTTask>& child) const
{
    return (this->children.find(child) != -1);
}

void BTTask::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("is_root"), &BTTask::is_root);
	ClassDB::bind_method(D_METHOD("get_root"), &BTTask::get_root);
	ClassDB::bind_method(D_METHOD("get_child_count"), &BTTask::get_child_count);
	ClassDB::bind_method(D_METHOD("add_child", "child"), &BTTask::add_child);
	ClassDB::bind_method(D_METHOD("add_child_at_index", "index", "child"), &BTTask::add_child_at_index);
	ClassDB::bind_method(D_METHOD("remove_child", "child"), &BTTask::remove_child);
	ClassDB::bind_method(D_METHOD("remove_child_at_index", "index"), &BTTask::remove_child_at_index);
	ClassDB::bind_method(D_METHOD("has_child", "child"), &BTTask::has_child);

    ClassDB::bind_method(D_METHOD("set_actor", "actor"), &BTTask::set_actor);
    ClassDB::bind_method(D_METHOD("get_actor"), &BTTask::get_actor);
    ClassDB::bind_method(D_METHOD("set_children", "children"), &BTTask::set_children);
    ClassDB::bind_method(D_METHOD("get_children"), &BTTask::get_children);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "actor"), "set_actor", "get_actor");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "children"), "set_children", "get_children");

    
    BIND_ENUM_CONSTANT(Status::FRESH);
    BIND_ENUM_CONSTANT(Status::RUNNING);
    BIND_ENUM_CONSTANT(Status::SUCCESS);
    BIND_ENUM_CONSTANT(Status::FAILURE);

}