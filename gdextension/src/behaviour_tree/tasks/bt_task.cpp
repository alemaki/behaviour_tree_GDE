#include "bt_task.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
BTTask::BTTask()
{
    this->actor = nullptr;
    this->parent = nullptr;
    this->status = Status::FRESH;
    this->custom_name = "";
}

void BTTask::set_actor(godot::Node* actor)
{
    this->actor = actor;
}

void BTTask::set_parent(godot::Ref<BTTask> parent)
{
    this->parent = parent.ptr();
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

void BTTask::set_custom_name(const godot::String& name)
{
    this->custom_name = name;
}

void BTTask::set_children(const godot::Array& children)
{
    for (godot::Ref<BTTask> child : this->children)
    {
        child->set_parent(nullptr);
    }

    this->children.clear();

    int size = children.size();
    int invalid_refs = 0;
    this->children.resize(size);

    for (int index = 0; index < size; index++)
    {
        godot::Ref<BTTask> child = children[index];
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

void BTTask::set_status(BTTask::Status status) 
{
    this->status = status;
}

bool BTTask::is_root() const
{
    return (this->parent == nullptr);
}

godot::Ref<BTTask> BTTask::get_root() const
{
    const BTTask* root(this);
    while(root->parent != nullptr)
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

void BTTask::add_child_at_index(godot::Ref<BTTask> child, int index)
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

bool BTTask::has_running_child() const
{
    for (godot::Ref<BTTask> child : this->children)
    {
        if (child->get_status() == Status::RUNNING)
        {
            return true;
        }
    }
    return false;
}

BTTask::Status BTTask::execute(double delta)
{
    // either fresh or already ran before
	if (this->status != RUNNING)
    {
		// Reset children status if already ran before.
		if (this->status != FRESH)
        {
			for (int i = 0; i < get_child_count(); i++)
            {
				this->children.get(i)->abort();
			}
		}
		this->_enter();
	}

	this->status = this->_tick(delta);

    // done after _tick
	if (this->status != RUNNING)
    {
		this->_exit();
	}
	return this->status;
}

void BTTask::abort()
{
	for (int i = 0; i < children.size(); i++)
    {
		this->get_child(i)->abort();
	}
	if (this->status == RUNNING)
    {
		this->_exit();
	}
	this->status = FRESH;
}

void BTTask::_setup()
{

}

void BTTask::_enter()
{
    
}

void BTTask:: _exit()
{

}

BTTask::Status BTTask::_tick(double delta)
{
    return Status::FAILURE;
}

void BTTask::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("is_root"), &BTTask::is_root);
	ClassDB::bind_method(D_METHOD("get_root"), &BTTask::get_root);
	ClassDB::bind_method(D_METHOD("get_child_count"), &BTTask::get_child_count);
	ClassDB::bind_method(D_METHOD("add_child", "child"), &BTTask::add_child);
	ClassDB::bind_method(D_METHOD("add_child_at_index", "child", "index"), &BTTask::add_child_at_index);
	ClassDB::bind_method(D_METHOD("remove_child", "child"), &BTTask::remove_child);
	ClassDB::bind_method(D_METHOD("remove_child_at_index", "index"), &BTTask::remove_child_at_index);
	ClassDB::bind_method(D_METHOD("has_child", "child"), &BTTask::has_child);

    ClassDB::bind_method(D_METHOD("set_parent", "parent"), &BTTask::set_parent);
    ClassDB::bind_method(D_METHOD("get_parent"), &BTTask::get_parent);
    ClassDB::bind_method(D_METHOD("set_custom_name", "name"), &BTTask::set_custom_name);
    ClassDB::bind_method(D_METHOD("get_custom_name"), &BTTask::get_custom_name);
    ClassDB::bind_method(D_METHOD("set_actor", "actor"), &BTTask::set_actor);
    ClassDB::bind_method(D_METHOD("get_actor"), &BTTask::get_actor);
    ClassDB::bind_method(D_METHOD("set_status", "status"), &BTTask::set_status);
    ClassDB::bind_method(D_METHOD("get_status"), &BTTask::get_status);
    ClassDB::bind_method(D_METHOD("set_children", "children"), &BTTask::set_children);
    ClassDB::bind_method(D_METHOD("get_children"), &BTTask::get_children);

    /* NOTE: Apparently the engine gives segmentation fault if property info about the objects isn't filled.
    * Only happens when there are other registered objects that inherit from this class 
    */
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parent", PROPERTY_HINT_NONE, "Parent of the task.", PROPERTY_USAGE_NONE), "set_parent", "get_parent");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "custom_name", PROPERTY_HINT_RESOURCE_TYPE, "Name of the task.", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL), "set_custom_name", "get_custom_name");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "actor", PROPERTY_HINT_RESOURCE_TYPE, "Actor to be controlled.", PROPERTY_USAGE_NONE), "set_actor", "get_actor");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "children", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL), "set_children", "get_children");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "status", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_status");
    
    BIND_ENUM_CONSTANT(Status::FRESH);
    BIND_ENUM_CONSTANT(Status::RUNNING);
    BIND_ENUM_CONSTANT(Status::SUCCESS);
    BIND_ENUM_CONSTANT(Status::FAILURE);

}
