#ifndef BT_FSM_HPP
#define BT_FSM_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/hash_set.hpp>

class FSM : public godot::Node
{
    GDCLASS(FSM, godot::Node);

private: 
    godot::String current_state = "";
    godot::HashMap<godot::String, godot::HashSet<godot::String>> transitions;

public:

    void set_initial_state(godot::String state);
    _FORCE_INLINE_ godot::String get_state() const
    {
        return this->current_state;
    }

    void add_transition(godot::String from, godot::String to);
    bool transition_to(godot::String state);

protected:
    static void _bind_methods();
};

#endif /* BT_FSM_HPP */