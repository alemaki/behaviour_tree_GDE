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

    ClassDB::bind_method(D_METHOD("set_value", "value"), &BTCheckInt::set_value);
    ClassDB::bind_method(D_METHOD("get_value"), &BTCheckInt::get_value);
    ClassDB::bind_method(D_METHOD("set_var_name", "var_name"), &BTCheckInt::set_var_name);
    ClassDB::bind_method(D_METHOD("get_var_name"), &BTCheckInt::get_var_name);

    ADD_PROPERTY(PropertyInfo(Variant::Type::INT, "value"), "set_value", "get_value");
    ADD_PROPERTY(PropertyInfo(Variant::Type::STRING_NAME, "var_name"), "set_var_name", "get_var_name");


}