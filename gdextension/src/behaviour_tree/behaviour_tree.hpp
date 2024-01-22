#ifndef BEHAVIOR_TREE_HPP
#define BEHAVIOR_TREE_HPP

#include <godot_cpp/classes/resource.hpp>
#include "behaviour_tree/tasks/bt_action.hpp"

class BehaviorTree : public godot::Resource
{
    GDCLASS(BehaviorTree, godot::Resource);

private:
    godot::String description;
    godot::Ref<BTTask> root_task;

public:
    void set_description(const godot::String &description);
    _FORCE_INLINE_ godot::String get_description() const;
    void set_root_task(godot::Ref<BTTask> task);
    _FORCE_INLINE_ godot::Ref<BTTask> get_root_task() const;


protected:
    static void _bind_methods();
};

#endif // BEHAVIOR_TREE_HPP