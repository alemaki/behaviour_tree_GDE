#include "bt_check_int_equal.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

BTTask::Status BTCheckIntEqual::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->var_name, godot::Variant(), this->complain);

    TASK_SUCCEED_COND(var == godot::Variant(this->get_value()));

    return BTTask::Status::FAILURE;
}

void BTCheckIntEqual::set_value(int value)
{
    this->value = value;
}

void BTCheckIntEqual::set_var_name(const godot::StringName& var_name)
{
    this->var_name = var_name;
}

void BTCheckIntEqual::set_complain(bool complain)
{
    this->complain = complain;
}


void BTCheckIntEqual::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_value", "value"), &BTCheckIntEqual::set_value);
    ClassDB::bind_method(D_METHOD("get_value"), &BTCheckIntEqual::get_value);
    ClassDB::bind_method(D_METHOD("set_var_name", "var_name"), &BTCheckIntEqual::set_var_name);
    ClassDB::bind_method(D_METHOD("get_var_name"), &BTCheckIntEqual::get_var_name);
    ClassDB::bind_method(D_METHOD("set_complain", "complain"), &BTCheckIntEqual::set_complain);

    ADD_PROPERTY(PropertyInfo(Variant::Type::INT, "value"), "set_value", "get_value");
    ADD_PROPERTY(PropertyInfo(Variant::Type::STRING_NAME, "var_name"), "set_var_name", "get_var_name");
    ADD_PROPERTY(PropertyInfo(Variant::Type::BOOL, "complain"), "set_complain", "get_complain");


}