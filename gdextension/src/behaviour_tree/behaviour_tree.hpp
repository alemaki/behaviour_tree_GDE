#ifndef BEHAVIOUR_TREE_HPP
#define BEHAVIOUR_TREE_HPP

#include <godot_cpp/classes/resource.hpp>
#include "behaviour_tree/tasks/bt_action.hpp"

class BehaviourTree : public godot::Node
{
    GDCLASS(BehaviourTree, godot::Node);

private:
    godot::String description;
    godot::Vector<godot::Ref<BTTask>> all_tasks;
    godot::Ref<BTTask> root_task;

public:
    void set_description(const godot::String &description);
    _FORCE_INLINE_ godot::String get_description() const
    {
        return this->description;
    }
    void set_root_task(godot::Ref<BTTask> task);
    _FORCE_INLINE_ godot::Ref<BTTask> get_root_task() const
    {
        return this->root_task;
    }

    void add_task(godot::Ref<BTTask> task);
    void remove_task(godot::Ref<BTTask> task);
    void clear_tasks();
    godot::Array get_tasks() const
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



protected:
    static void _bind_methods();
};

#endif // BEHAVIOUR_TREE_HPP