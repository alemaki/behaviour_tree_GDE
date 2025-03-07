#include "state.hpp"

void State::_enter_state()
{
    this->emit_signal("entered");
}

void State::_process_state(double delta) const
{
    for (int i = 0; i < process_callables.size(); i++)
    {
        godot::Callable callable = process_callables[i];
        if (!callable.is_valid())
        {
            /* TODO: error? remove? */
            continue;
        }

        godot::Object *target = callable.get_object();
        /* TODO: remove otherwise? */
        if (target && !target->is_queued_for_deletion())
        {
            callable.call(delta);
        }
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
