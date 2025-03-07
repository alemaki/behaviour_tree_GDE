#ifndef BT_FSM_HPP
#define BT_FSM_HPP

#include "behaviour_tree/utils/macros.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_set.hpp>

#include "finite_state_machine/state.hpp"

class FSM : public godot::Node
{
    GDCLASS(FSM, godot::Node);

private:
    State* initial_state = nullptr;
    State* current_state = nullptr;

    godot::HashSet<State*> states;

public:
    CREATE_GETTER_SETTER_DEFAULT(State*, initial_state);

    _FORCE_INLINE_ State* get_state() const
    {
        return this->current_state;
    }

    State* create_state();
    bool transition_to_state(State* state);

    void process_state(double delta)  const;

    _FORCE_INLINE_ bool can_transition_to_state(State* state) const
    {
        return this->states.has(state);
    }

    void initialize();

protected:
    static void _bind_methods();
};

#endif /* BT_FSM_HPP */