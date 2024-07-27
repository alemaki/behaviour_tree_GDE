#include "bt_transition_fsm.hpp"
#include "finite_state_machine/fsm.hpp"

BTTask::Status BTTransitionFSM::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->fsm_name, godot::Variant(), this->complain);
    FSM* fsm = godot::Object::cast_to<FSM>(var);

    TASK_FAIL_COND_COMP_MSG(fsm == nullptr, this->get_name() + ": \"" + this->fsm_name + "\" is not a FSM.");
    TASK_FAIL_COND_COMP_MSG(!(fsm->can_transition_to(this->to_state)), this->get_name() + ": \"" + this->fsm_name + "\" cannot transition from state \"" +  fsm->get_state() + "\" to \"" + this->to_state + "\'.");
    
    fsm->transition_to(this->to_state);
    TASK_SUCCEED();
}

void BTTransitionFSM::set_to_state(const godot::String& state)
{
    this->to_state = to_state;
}

void BTTransitionFSM::set_fsm_name(const godot::StringName& fsm_name)
{
    this->fsm_name = fsm_name;
}

void BTTransitionFSM::set_complain(bool complain)
{
    this->complain = complain;
}

void BTTransitionFSM::_bind_methods()
{
    using namespace godot;
}
