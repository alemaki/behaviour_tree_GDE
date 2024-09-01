#include "fsm.hpp"
#include "behaviour_tree/utils/macros.hpp"

void FSM::add_state(const godot::String& state)
{
    this->states.insert(state);
}

bool FSM::transition_to(const godot::String& state)
{
    if (this->can_transition_to(state))
    {
        current_state = state;
        return true;
    }
    return false;
}

void FSM::_ready()
{
    this->current_state = this->initial_state;
    if (this->current_state == "")
    {
        godot::UtilityFunctions::printerr(this->get_name(), ": initial state was not set.");
        ERR_FAIL_COND(this->states.size() == 0);
        this->current_state = this->states.begin().operator*();
    }
}

void FSM::set_initial_state(const godot::String& state)
{
    if (!(this->states.has(state)))
    {
        this->add_state(state);
    }
    this->initial_state = state;
    if (this->current_state == "")
    {
        this->current_state = state;
    }
}

void FSM::set_states(const godot::Array& states)
{
    this->states.clear();
    
    int size = states.size();
    for (int i = 0; i < size; i++)
    {
        this->states.insert(states[i]);
    }
}

godot::Array FSM::get_states() const
{
    godot::Array result;
    result.resize(this->states.size());
    int i = 0;
    for (godot::String state : this->states)
    {
        result[i] = state;
        i++;
    }
    return result;
}

void FSM::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("get_state"), &FSM::get_state);
    ClassDB::bind_method(D_METHOD("add_state", "state"), &FSM::add_state);
    ClassDB::bind_method(D_METHOD("transition_to", "state"), &FSM::transition_to);
    ClassDB::bind_method(D_METHOD("can_transition_to", "state"), &FSM::transition_to);

    
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(FSM, ARRAY, states);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(FSM, STRING, initial_state);
}