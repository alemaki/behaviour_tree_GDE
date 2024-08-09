#include "bt_transition_fsm.hpp"
#include "finite_state_machine/fsm.hpp"

BTTask::Status BTTransitionFSM::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->fsm_name, godot::Variant(), this->is_complain_enabled());
    FSM* fsm = godot::Object::cast_to<FSM>(var);

    TASK_FAIL_COND_COMP_MSG(fsm == nullptr, this->get_name() + ": \"" + this->fsm_name + "\" is not a FSM.");
    TASK_FAIL_COND_COMP_MSG(!(fsm->can_transition_to(this->to_state)), this->get_name() + ": \"" + this->fsm_name + "\" cannot transition from state \"" +  fsm->get_state() + "\" to \"" + this->to_state + "\".");
    
    fsm->transition_to(this->to_state);
    TASK_SUCCEED();
}

void BTTransitionFSM::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_to_state", "to_state"), &BTTransitionFSM::set_to_state);
    ClassDB::bind_method(D_METHOD("get_to_state"), &BTTransitionFSM::get_to_state);
    ClassDB::bind_method(D_METHOD("set_fsm_name", "fsm_name"), &BTTransitionFSM::set_fsm_name);
    ClassDB::bind_method(D_METHOD("get_fsm_name"), &BTTransitionFSM::get_fsm_name);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "to_state"), "set_to_state", "get_to_state");
    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "fsm_name"), "set_fsm_name", "get_fsm_name");
}
