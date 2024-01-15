#ifndef BT_TASK_HPP
#define BT_TASK_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>

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
    godot::Node* actor;
    godot::Vector<godot::Ref<BTTask>> children;

    godot::Array get_children() const;
    void set_children(godot::Array array);

public:
    BTTask();

    _FORCE_INLINE_ godot::Node* get_actor() const;
    _FORCE_INLINE_ Status get_status() const;
    void set_actor(godot::Node* actor);
    void set_status(BTTask::Status status);

    _FORCE_INLINE_ godot::Ref<BTTask> get_parent() const;
    _FORCE_INLINE_ int get_child_count()const;
    bool is_root() const;
    
	godot::Ref<BTTask> get_root() const;

	void add_child(godot::Ref<BTTask> child);
	void add_child_at_index(godot::Ref<BTTask> child, int index);
	void remove_child(godot::Ref<BTTask> child);
	void remove_child_at_index(int index);

	_FORCE_INLINE_ bool has_child(const godot::Ref<BTTask> &p_child) const;

protected:
    static void _bind_methods();

};
VARIANT_ENUM_CAST(BTTask::Status);

#endif // BT_TASK_HPP