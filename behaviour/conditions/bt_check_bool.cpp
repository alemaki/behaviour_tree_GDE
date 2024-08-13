#include "bt_check_bool.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

BTTask::Status BTCheckBool::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->var_name, godot::Variant(), this->is_complain_enabled());

    if (this->is_complain_enabled() && var.get_type() != godot::Variant::Type::BOOL && var.get_type() != godot::Variant::Type::NIL)
    {
        godot::UtilityFunctions::printerr(this->get_name() + ": \"" + this->var_name + "\" is not a bool.");
    }

    TASK_SUCCEED_COND(var == godot::Variant(this->get_check_true()));

    TASK_FAIL();
}

void BTCheckBool::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckBool, BOOL, check_true);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckBool, STRING_NAME, var_name);
}