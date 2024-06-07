#include "bt_check_int.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

BTTask::Status BTCheckInt::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->var_name, godot::Variant(), this->complain);

    if (this->complain && var.get_type() != godot::Variant::Type::INT && var.get_type() != godot::Variant::Type::NIL)
    {
        godot::UtilityFunctions::printerr(this->get_name() + ": \"" + this->var_name + "\" is not an integer.");
    }

    TASK_SUCCEED_COND(var == godot::Variant(this->get_value()));

    return BTTask::Status::FAILURE;
}

void BTCheckInt::set_value(int value)
{
    this->value = value;
}

void BTCheckInt::set_var_name(const godot::StringName& var_name)
{
    this->var_name = var_name;
}

void BTCheckInt::set_complain(bool complain)
{
    this->complain = complain;
}


void BTCheckInt::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_value", "value"), &BTCheckInt::set_value);
    ClassDB::bind_method(D_METHOD("get_value"), &BTCheckInt::get_value);
    ClassDB::bind_method(D_METHOD("set_var_name", "var_name"), &BTCheckInt::set_var_name);
    ClassDB::bind_method(D_METHOD("get_var_name"), &BTCheckInt::get_var_name);
    ClassDB::bind_method(D_METHOD("set_complain", "complain"), &BTCheckInt::set_complain);
    ClassDB::bind_method(D_METHOD("get_complain"), &BTCheckInt::get_complain);

    ADD_PROPERTY(PropertyInfo(Variant::Type::INT, "value"), "set_value", "get_value");
    ADD_PROPERTY(PropertyInfo(Variant::Type::STRING_NAME, "var_name"), "set_var_name", "get_var_name");
    ADD_PROPERTY(PropertyInfo(Variant::Type::BOOL, "complain"), "set_complain", "get_complain");


}