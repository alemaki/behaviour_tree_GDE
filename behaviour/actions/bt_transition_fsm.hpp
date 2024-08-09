#ifndef BT_TRANSITION_FSM_HPP
#define BT_TRANSITION_FSM_HPP

#include "behaviour_tree/tasks/bt_action.hpp"

class BTTransitionFSM : public BTAction
{
    GDCLASS(BTTransitionFSM, BTAction);

private:
    godot::String to_state;
    godot::StringName fsm_name;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
	CREATE_GETTER_SETTER_STRING_DEFAULT(to_state);
    CREATE_GETTER_SETTER_STRINGNAME_DEFAULT(fsm_name);

protected:
    static void _bind_methods();
};

#endif /* BT_TRANSITION_FSM_HPP */