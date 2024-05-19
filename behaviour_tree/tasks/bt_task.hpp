#ifndef BT_TASK_HPP
#define BT_TASK_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>

#include "behaviour_tree/utils/macros.hpp"

class BTTask : public godot::Resource
{
    GDCLASS(BTTask, godot::Resource);

public:
    enum Status 
    {
        FRESH,
        RUNNING,
        SUCCESS,
        FAILURE,
    };

private:
    Status status;
    BTTask* parent;
    godot::String custom_name;
    godot::Node* actor;
    godot::Vector<godot::Ref<BTTask>> children;

protected:
    virtual void _setup();
    virtual void _enter();
    virtual void _exit();
    virtual Status _tick(double delta);
    
public:
    BTTask();
    void set_custom_name(const godot::String& name);
    _FORCE_INLINE_ godot::String get_custom_name() const
    {
        return this->custom_name;
    }
    void set_actor(godot::Node* actor);
    _FORCE_INLINE_ godot::Node* get_actor() const
    {
        return this->actor;
    }
    void set_status(BTTask::Status status);
    _FORCE_INLINE_ Status get_status() const
    {
        return this->status;
    }

    void set_parent(godot::Ref<BTTask> parent);
    _FORCE_INLINE_ godot::Ref<BTTask> get_parent() const
    {
        return godot::Ref<BTTask>(this->parent);
    }
    void set_children(const godot::Array& children);
    // NOTE: can't return godot::Vector<godot::Ref<BTTask>>. Investigate.
    godot::Array get_children() const;
    _FORCE_INLINE_ int get_child_count() const
    {
        return this->children.size();
    }
    _FORCE_INLINE_ godot::Ref<BTTask> get_child(int index) const
    {
        return godot::Ref<BTTask>(this->children[index]);
    }
    bool is_root() const;
    
	godot::Ref<BTTask> get_root() const;

	void add_child(godot::Ref<BTTask> child);
	void add_child_at_index(godot::Ref<BTTask> child, int index);
	void remove_child(godot::Ref<BTTask> child);
	void remove_child_at_index(int index);
    void swap_child(godot::Ref<BTTask> old_child, godot::Ref<BTTask> new_child);

	_FORCE_INLINE_ bool has_child(const godot::Ref<BTTask> child) const
    {
        return (this->children.find(child) != -1);
    }
    _FORCE_INLINE_ int get_child_index(const godot::Ref<BTTask> child) const
    {
        return this->children.find(child);
    }
    bool has_running_child() const;
    virtual BTTask::Status execute(double delta);
    virtual void abort();

    virtual godot::Ref<BTTask> clone() const;

    void initialize(godot::Node* actor);

protected:
    static void _bind_methods();

};

VARIANT_ENUM_CAST(BTTask::Status);

#endif // BT_TASK_HPP