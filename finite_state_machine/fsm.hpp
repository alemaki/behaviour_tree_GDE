#ifndef BT_FSM_HPP
#define BT_FSM_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_set.hpp>

class FSM : public godot::Node
{
    GDCLASS(FSM, godot::Node);

private: 
    godot::String initial_state;
    godot::String current_state;

    godot::HashSet<godot::String> states;

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

    void set_states(const godot::Array& transitions);
    godot::Array get_states() const;

    void add_state(const godot::String& state);
    bool transition_to(const godot::String& state);
    _FORCE_INLINE_ bool can_transition_to(const godot::String& state) const
    {
        return this->states.has(state);
    }

    void _ready() override;

protected:
    static void _bind_methods();
};

#endif /* BT_FSM_HPP */