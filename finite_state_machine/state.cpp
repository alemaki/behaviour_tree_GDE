#include "state.hpp"

void State::_enter()
{
    this->emit_signal("entered");
}

void State::_exit()
{
    this->emit_signal("exited");
}

void State::add_enter_callable(const godot::Callable& callable)
{
    ERR_FAIL_COND(!callable.is_valid());
    this->connect("entered", callable);
}

void State::add_exit_callable(const godot::Callable& callable)
{
    ERR_FAIL_COND(!callable.is_valid());
    this->connect("exited", callable);
}

void State::_bind_methods()
{
    using namespace godot;

    ADD_SIGNAL(MethodInfo("entered"));
    ADD_SIGNAL(MethodInfo("exited"));
}
