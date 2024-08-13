#include "fsm.hpp"
#include "behaviour_tree/utils/macros.hpp"

void FSM::add_transition(const godot::String& from, const godot::String& to)
{
    if (!(this->transitions.has(from)))
    {
        this->transitions.insert(from, godot::HashSet<godot::String>());
    }
    this->transitions[from].insert(to);
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
}

void FSM::set_initial_state(const godot::String& state)
{
    this->initial_state = state;
    if (this->current_state == "")
    {
        this->current_state = state;
    }
}

void FSM::set_transitions(const godot::Dictionary& transitions)
{
    this->transitions.clear();
    
    godot::Array keys = transitions.keys();
    int keys_size = keys.size();
    for (int i = 0; i < keys_size; i++)
    {
        godot::String key = keys[i];
        godot::Array values = transitions[key];
        int values_size = values.size();
        godot::HashSet<godot::String> result;
        for(int j = 0; j < values_size; j++)
        {
            result.insert(values[j]);
        }
        this->transitions.insert(key, result);
    }
}

godot::Dictionary FSM::get_transitions() const
{
    godot::Dictionary result;
    for (godot::KeyValue<godot::String, godot::HashSet<godot::String>> key_value : this->transitions)
    {
        godot::Array values;
        for (const godot::String& value : key_value.value)
        {
            values.push_back(value);
        }
        result[key_value.key] = values;
    }
    return result;
}

void FSM::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("get_state"), &FSM::get_state);
    ClassDB::bind_method(D_METHOD("add_transition", "from", "to"), &FSM::add_transition);
    ClassDB::bind_method(D_METHOD("transition_to", "state"), &FSM::transition_to);
    ClassDB::bind_method(D_METHOD("can_transition_to", "state"), &FSM::transition_to);

    
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(FSM, STRING, initial_state);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(FSM, DICTIONARY, transitions);
}