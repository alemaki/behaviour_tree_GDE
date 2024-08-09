#ifndef BT_CHECK_FSM_STATE_HPP
#define BT_CHECK_FSM_STATE_HPP

#include "behaviour_tree/tasks/bt_condition.hpp"

class BTCheckFSMState : public BTCondition
{
    GDCLASS(BTCheckFSMState, BTCondition);

private:
    godot::String state;
    godot::StringName fsm_name;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    CREATE_GETTER_SETTER_STRING_DEFAULT(state);
    CREATE_GETTER_SETTER_STRINGNAME_DEFAULT(fsm_name);

protected:
    static void _bind_methods();
};

#endif /* BT_CHECK_FSM_STATE_HPP */