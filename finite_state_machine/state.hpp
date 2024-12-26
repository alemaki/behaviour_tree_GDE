#ifndef STATE_HPP
#define STATE_HPP

#include <godot_cpp/classes/node.hpp>

class State : public godot::Node
{
    GDCLASS(State, godot::Node);

public:
    void add_enter_callable(const godot::Callable& callable);
    void add_exit_callable(const godot::Callable& callable);

    void _ready() override;

protected:
    static void _bind_methods();
};

#endif /* BT_FSM_HPP */