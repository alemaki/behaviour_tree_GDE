#ifndef BT_TASK_HPP
#define BT_TASK_HPP

#include <godot_cpp/classes/resource.hpp>

class BTTask : public godot::Resource
{
    GDCLASS(BTTask, godot::Resource);

    enum Status
    {
        FRESH,
        RUNNING,
        FAILURE,
        SUCCESS,
    };
private:
    Status status;
    BTTask* parent;
    godot::Node* actor;
    godot::Vector<godot::Ref<BTTask>> children;

    godot::Array get_children() const;
    void set_children(godot::Array children);

public:
    godot::Node* get_actor() const;
    void set_actor(godot::Node* actor);

    godot::Ref<BTTask> get_parent() const;
    Status get_status() const;
    int get_child_count()const;

    bool is_root() const;
    
	godot::Ref<BTTask> get_root() const;

	void add_child(godot::Ref<BTTask> child);
	void add_child_at_index(int index, godot::Ref<BTTask> child);
	void remove_child(godot::Ref<BTTask> child);
	void remove_child_at_index(int index);

	bool has_child(const godot::Ref<BTTask> &p_child) const;

protected:
    static void _bind_methods();

}

#endif // BT_TASK_HPP