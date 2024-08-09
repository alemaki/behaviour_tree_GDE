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

    ClassDB::bind_method(D_METHOD("set_check_true", "value"), &BTCheckBool::set_check_true);
    ClassDB::bind_method(D_METHOD("get_check_true"), &BTCheckBool::get_check_true);
    ClassDB::bind_method(D_METHOD("set_var_name", "var_name"), &BTCheckBool::set_var_name);
    ClassDB::bind_method(D_METHOD("get_var_name"), &BTCheckBool::get_var_name);

    ADD_PROPERTY(PropertyInfo(Variant::Type::BOOL, "is_true"), "set_check_true", "get_check_true");
    ADD_PROPERTY(PropertyInfo(Variant::Type::STRING_NAME, "var_name"), "set_var_name", "get_var_name");


}