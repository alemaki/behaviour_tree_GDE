#include "bt_check_int.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

BTTask::Status BTCheckInt::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->var_name, godot::Variant(), this->is_complain_enabled());

    if (this->is_complain_enabled()&& var.get_type() != godot::Variant::Type::INT && var.get_type() != godot::Variant::Type::NIL)
    {
        godot::UtilityFunctions::printerr(this->get_name() + ": \"" + this->var_name + "\" is not an integer.");
    }

    TASK_SUCCEED_COND(var == godot::Variant(this->get_value()));

    TASK_FAIL();
}

void BTCheckInt::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckInt, INT, value);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckInt, STRING_NAME, var_name);
}