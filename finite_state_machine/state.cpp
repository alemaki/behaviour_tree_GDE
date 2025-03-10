#include "state.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

void State::_enter_state()
{
    this->emit_signal("entered");
}

void State::_process_state(double delta) const
{
    for (int i = 0; i < process_callables.size(); i++)
    {
        godot::Callable callable = process_callables[i];
        ERR_CONTINUE_MSG(!callable.is_valid(), vformat("State '%s' has invalid callable: '%s'", this->get_name(), callable));
        callable.call(delta);
    }
}

void State::_exit_state()
{
    this->emit_signal("exited");
}

State* State::add_enter_callable(const godot::Callable& callable)
{
    ERR_FAIL_COND_V(!callable.is_valid(), this);
    this->connect("entered", callable);
    return this;
}

State* State::add_process_callable(const godot::Callable& callable)
{
    ERR_FAIL_COND_V(!callable.is_valid(), this);
    this->process_callables.push_back(callable);
    return this;
}

State* State::add_exit_callable(const godot::Callable& callable)
{
    ERR_FAIL_COND_V(!callable.is_valid(), this);
    this->connect("exited", callable);
    return this;
}

void State::_bind_methods()
{
    using namespace godot;

    ADD_SIGNAL(MethodInfo("entered"));
    ADD_SIGNAL(MethodInfo("exited"));
}
