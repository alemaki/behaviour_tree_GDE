#include "fsm.hpp"
#include "behaviour_tree/utils/macros.hpp"

State* FSM::create_state()
{
    State* new_state = memnew(State);
    this->add_child(new_state);
    return new_state;
}

bool FSM::transition_to_state(State* state)
{
    ERR_FAIL_NULL_V(state, false);
    if (this->can_transition_to_state(state))
    {
        this->current_state->_exit();
        current_state = state;
        this->current_state->_enter();
        return true;
    }
    return false;
}

void FSM::initialize()
{
    ERR_FAIL_NULL_MSG(this->initial_state, godot::String(this->get_name()) + ": initial state was not set.");
    bool initial_state_is_child = false;
    const godot::Array& children = this->get_children();
    for (int i = 0, size = children.size(); i < size; i++)
    {
        State* state = godot::Object::cast_to<State>(children[i]);
        ERR_CONTINUE_MSG(state == nullptr, "FSM has non-state child.");
        this->states.insert(state);
        if(state == this->initial_state)
        {
            initial_state_is_child = true;
        }
    }

    if (initial_state_is_child)
    {
        this->current_state = initial_state;
    }
    else
    {
        godot::UtilityFunctions::printerr(this->get_name(), ": initial state is not child of FSM.");
        ERR_FAIL_COND(this->states.size() == 0);
        this->current_state = this->states.begin().operator*();
    }

    this->current_state->_enter();
}

void FSM::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("get_state"), &FSM::get_state);
    ClassDB::bind_method(D_METHOD("create_state"), &FSM::create_state);
    ClassDB::bind_method(D_METHOD("transition_to_state", "state"), &FSM::transition_to_state);
    ClassDB::bind_method(D_METHOD("can_transition_to_state", "state"), &FSM::can_transition_to_state);
    ClassDB::bind_method(D_METHOD("initialize"), &FSM::initialize);

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(FSM, STRING, initial_state);
}