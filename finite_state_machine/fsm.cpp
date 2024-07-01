#include "fsm.hpp"

void FSM::set_initial_state(godot::String state)
{
    if (this->current_state == "")
    {
        this->current_state = state;
    }
}

void FSM::add_transition(godot::String from, godot::String to)
{
    if (!(this->transitions.has(from)))
    {
        this->transitions.insert(from, godot::HashSet<godot::String>());
    }
    this->transitions[from].insert(to);
}

bool FSM::transition_to(godot::String state)
{
    if ((this->transitions.has(this->current_state)) && this->transitions[this->current_state].has(state))
    {
        current_state = state;
        return true;
    }
    return false;
}

void FSM::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_initial_state"), &FSM::set_initial_state);
    ClassDB::bind_method(D_METHOD("get_state"), &FSM::get_state);
    ClassDB::bind_method(D_METHOD("add_transition"), &FSM::add_transition);
    ClassDB::bind_method(D_METHOD("transition_to"), &FSM::transition_to);
}