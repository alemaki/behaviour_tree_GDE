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
protected:
    Status status;
    godot::StringName name;
    godot::Node* actor;
    godot::Vector<godot::Ref<BTTask>> children;


public:
    BTTask();
    ~BTTask();


protected:
    static void _bind_methods();

}

#endif // BT_TASK_HPP