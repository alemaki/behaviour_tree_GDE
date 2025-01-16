#ifndef STATE_HPP
#define STATE_HPP

#include <godot_cpp/classes/node.hpp>

#ifndef FSM_FORWARD
#define FSM_FORWARD
class FSM;
#endif /* FSM_FORWARD */

class State : public godot::Node
{
    GDCLASS(State, godot::Node);

    friend FSM;

private:
    void _enter();
    void _exit();

public:
    void add_enter_callable(const godot::Callable& callable);
    void add_exit_callable(const godot::Callable& callable);

protected:
    static void _bind_methods();
};

#endif /* BT_FSM_HPP */