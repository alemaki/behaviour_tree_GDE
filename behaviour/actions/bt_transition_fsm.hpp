#ifndef BT_TRANSITION_FSM_HPP
#define BT_TRANSITION_FSM_HPP

#include "behaviour_tree/tasks/bt_action.hpp"

class BTTransitionFSM : public BTAction
{
    GDCLASS(BTTransitionFSM, BTAction);

private:
    godot::String to_state;
    godot::StringName fsm_name;
    bool complain = true;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    void set_to_state(const godot::String& state);
    _FORCE_INLINE_ godot::String get_to_state() const
    {
        return this->to_state;
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

#endif /* BT_TRANSITION_FSM_HPP */