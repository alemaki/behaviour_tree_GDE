#include "bt_check_fsm_state.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include "finite_state_machine/fsm.hpp"

BTTask::Status BTCheckFSMState::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->fsm_name, godot::Variant(), this->complain);
    const FSM* fsm = godot::Object::cast_to<FSM>(var);

    TASK_FAIL_COND_COMP_MSG(fsm == nullptr, this->get_name() + ": \"" + this->fsm_name + "\" is not a FSM.")

    TASK_SUCCEED_COND(fsm->get_state() == this->state);

    TASK_FAIL();
}

void BTCheckFSMState::set_state(godot::String state)
{
    this->state = state;
}

void BTCheckFSMState::set_fsm_name(const godot::StringName& fsm_name)
{
    this->fsm_name = fsm_name;
}

void BTCheckFSMState::set_complain(bool complain)
{
    this->complain = complain;
}


void BTCheckFSMState::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_state", "state"), &BTCheckFSMState::set_state);
    ClassDB::bind_method(D_METHOD("get_state"), &BTCheckFSMState::get_state);
    ClassDB::bind_method(D_METHOD("set_fsm_name", "fsm_name"), &BTCheckFSMState::set_fsm_name);
    ClassDB::bind_method(D_METHOD("get_fsm_name"), &BTCheckFSMState::get_fsm_name);
    ClassDB::bind_method(D_METHOD("set_complain", "complain"), &BTCheckFSMState::set_complain);
    ClassDB::bind_method(D_METHOD("get_complain"), &BTCheckFSMState::get_complain);

    ADD_PROPERTY(PropertyInfo(Variant::Type::STRING, "state"), "set_state", "get_state");
    ADD_PROPERTY(PropertyInfo(Variant::Type::STRING_NAME, "fsm_name"), "set_fsm_name", "get_fsm_name");
    ADD_PROPERTY(PropertyInfo(Variant::Type::BOOL, "complain"), "set_complain", "get_complain");
}