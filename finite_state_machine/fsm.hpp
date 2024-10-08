#ifndef BT_FSM_HPP
#define BT_FSM_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/hash_set.hpp>

class FSM : public godot::Node
{
    GDCLASS(FSM, godot::Node);

private: 
    godot::String initial_state;
    godot::String current_state;
    godot::HashMap<godot::String, godot::HashSet<godot::String>> transitions;

public:
    void set_initial_state(const godot::String& state);
    _FORCE_INLINE_ godot::String get_initial_state() const
    {
        return this->initial_state;
    }

    _FORCE_INLINE_ godot::String get_state() const
    {
        return this->current_state;
    }

    void set_transitions(const godot::Dictionary& transitions);
    godot::Dictionary get_transitions() const;

    void add_transition(const godot::String& from, const godot::String& to);
    bool transition_to(const godot::String& state);
    _FORCE_INLINE_ bool can_transition_to(const godot::String& state) const
    {
        return (this->transitions.has(this->current_state)) && this->transitions[this->current_state].has(state);
    }

    void _ready() override;

protected:
    static void _bind_methods();
};

#endif /* BT_FSM_HPP */