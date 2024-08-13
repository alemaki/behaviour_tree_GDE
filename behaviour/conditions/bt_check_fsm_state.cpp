#include "bt_check_fsm_state.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include "finite_state_machine/fsm.hpp"

BTTask::Status BTCheckFSMState::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->fsm_name, godot::Variant(), this->is_complain_enabled());
    const FSM* fsm = godot::Object::cast_to<FSM>(var);

    TASK_FAIL_COND_COMP_MSG(fsm == nullptr, this->get_name() + ": \"" + this->fsm_name + "\" is not a FSM.");

    TASK_SUCCEED_COND(fsm->get_state() == this->state);

    TASK_FAIL();
}

void BTCheckFSMState::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckFSMState, STRING, state);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckFSMState, STRING_NAME, fsm_name);
}