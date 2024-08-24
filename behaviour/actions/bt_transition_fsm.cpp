#include "bt_transition_fsm.hpp"
#include "finite_state_machine/fsm.hpp"

BTTask::Status BTTransitionFSM::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->fsm_name, godot::Variant(), this->is_complain_enabled());
    FSM* fsm = godot::Object::cast_to<FSM>(var);

    TASK_FAIL_COND_COMP_MSG(fsm == nullptr, "\"" + this->fsm_name + "\" is not a FSM.");
    TASK_FAIL_COND_COMP_MSG(!(fsm->can_transition_to(this->to_state)), "\"" + this->fsm_name + "\" cannot transition from state \"" +  fsm->get_state() + "\" to \"" + this->to_state + "\".");
    
    fsm->transition_to(this->to_state);
    TASK_SUCCEED();
}

void BTTransitionFSM::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTTransitionFSM, STRING, to_state);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTTransitionFSM, STRING_NAME, fsm_name);
}
