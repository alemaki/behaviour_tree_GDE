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
    godot::TypedArray<godot::Callable> process_callables;

private:
    void _enter_state();
    void _process_state(double delta) const;
    void _exit_state();

public:
    State* add_enter_callable(const godot::Callable& callable);
    State* add_process_callable(const godot::Callable& callable);
    State* add_exit_callable(const godot::Callable& callable);

protected:
    static void _bind_methods();
};

#endif /* BT_FSM_HPP */