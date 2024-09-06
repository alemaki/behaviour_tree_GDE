#ifndef BEHAVIOUR_TREE_HPP
#define BEHAVIOUR_TREE_HPP

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/rb_map.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"
#include "blackboard/blackboard.hpp"

class BehaviourTree : public godot::Node
{
    GDCLASS(BehaviourTree, godot::Node);

private:
    godot::String description;
    godot::RBMap<int, godot::Ref<BTTask>> task_map; // Using rb_map for back method
    godot::Ref<BTTask> root_task = nullptr;

private:
    godot::Dictionary get_task_map();
    void set_task_map(godot::Dictionary dict);

public:
    CREATE_GETTER_SETTER_STRING_DEFAULT(description);

    void set_root_task(godot::Ref<BTTask> task);
    _FORCE_INLINE_ godot::Ref<BTTask> get_root_task() const
    {
        return this->root_task;
    }
    
    void set_custom_name_task_by_ref(godot::Ref<BTTask> task, const godot::String& new_name);
    int get_valid_id() const;
    int get_task_id(godot::Ref<BTTask> task) const;
    godot::Ref<BTTask> get_task(int id) const;
    bool has_task(godot::Ref<BTTask> task) const;
    void add_task_by_ref(godot::Ref<BTTask> task);
    void add_task(int id, godot::Ref<BTTask> task);
    void remove_task_by_ref(godot::Ref<BTTask> task);
    void remove_task(int id);
    void detach_task_by_ref(godot::Ref<BTTask> task);
    void detach_task(int id); 
    void clear_tasks();
    godot::Array get_tasks() const;

    _FORCE_INLINE_ bool can_connect(godot::Ref<BTTask> parent, godot::Ref<BTTask> child) const
    {
        return (
            parent.is_valid() &&
            child.is_valid() &&
            /* TODO: remove has_task or leave it be? */
            has_task(parent) &&
            has_task(child) &&
            (child != parent) &&
            (parent->get_root() != child) &&
            !(child->get_parent().is_valid()) &&
            !(parent->has_child(child))
            );
    }

    _FORCE_INLINE_ bool can_disconnect(godot::Ref<BTTask> parent, godot::Ref<BTTask> child) const
    {
        return (parent.is_valid() && child.is_valid() && parent->has_child(child));
    }

    void connect_tasks(godot::Ref<BTTask> parent, godot::Ref<BTTask> child, int child_pos = 0);
    void disconnect_tasks(godot::Ref<BTTask> parent, godot::Ref<BTTask> child);
    void set_children_of_task(godot::Ref<BTTask> parent, godot::Array new_children);
    void swap_task_in(godot::Ref<BTTask> old_task, godot::Ref<BTTask> new_task);

    _FORCE_INLINE_ int get_task_count() const
    {
        return this->task_map.size();
    }

    godot::Ref<BTTask> instantiate(Node* actor, godot::Ref<Blackboard> blackboard) const;

protected:
    static void _bind_methods();
};

#endif // BEHAVIOUR_TREE_HPP