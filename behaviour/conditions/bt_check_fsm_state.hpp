#ifndef BT_CHECK_FSM_STATE_HPP
#define BT_CHECK_FSM_STATE_HPP

#include "behaviour_tree/tasks/bt_condition.hpp"

class BTCheckFSMState : public BTCondition
{
    GDCLASS(BTCheckFSMState, BTCondition);

private:
    godot::String state;
    godot::StringName fsm_name;
    bool complain = true;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    void set_state(const godot::String& state);
    _FORCE_INLINE_ godot::String get_state() const
    {
        return this->state;
    }
    void set_fsm_name(const godot::StringName& fsm_name);
    _FORCE_INLINE_ godot::StringName get_fsm_name() const
    {
        return this->fsm_name;
    }
    void set_complain(bool complain);
    _FORCE_INLINE_ bool get_complain() const
    {
        return this->complain;
    }

protected:
    static void _bind_methods();
};

#endif /* BT_CHECK_FSM_STATE_HPP */