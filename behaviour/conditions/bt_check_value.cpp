#include "bt_check_value.hpp"

#include <godot_cpp/variant/utility_functions.hpp>


BTTask::Status BTCheckValue::_tick(double delta)
{
    const godot::Variant var = this->get_blackboard()->get_var(this->var_name, godot::Variant(), this->is_complain_enabled());

    TASK_COMPLAIN_COND(var.get_type() != godot::Variant::Type::INT &&
        var.get_type() != godot::Variant::Type::FLOAT &&
        var.get_type() != godot::Variant::Type::STRING &&
        var.get_type() != godot::Variant::Type::STRING_NAME &&
        var.get_type() != godot::Variant::Type::BOOL &&
        var.get_type() != godot::Variant::Type::VECTOR2 &&
        var.get_type() != godot::Variant::Type::VECTOR3 &&
        var.get_type() != godot::Variant::Type::NIL, "\"" + this->var_name + "\" has an unsupported type.");

    TASK_SUCCEED_COND(
        (this->is_check_true_enabled() && var == godot::Variant(true)) ||
        (this->is_check_int_enabled() && var == godot::Variant(this->int_value)) ||
        (this->is_check_float_enabled() && var == godot::Variant(this->float_value)) ||
        (this->is_check_string_enabled() && var == godot::Variant(this->string_value)) ||
        (this->is_check_string_name_enabled() && var == godot::Variant(this->string_name_value)) ||
        (this->is_check_vector2_enabled() && this->vector2_value.is_equal_approx(var)) ||
        (this->is_check_vector3_enabled() && this->vector3_value.is_equal_approx(var))
    );

    TASK_FAIL();
}

void BTCheckValue::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckValue, STRING_NAME, var_name);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckValue, INT, int_value);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckValue, FLOAT, float_value);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckValue, STRING, string_value);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckValue, STRING_NAME, string_name_value);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckValue, VECTOR2, vector2_value);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTCheckValue, VECTOR3, vector3_value);

    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCheckValue, check_true_enabled);
    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCheckValue, check_int_enabled);
    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCheckValue, check_float_enabled);
    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCheckValue, check_string_enabled);
    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCheckValue, check_string_name_enabled);
    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCheckValue, check_vector2_enabled);
    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCheckValue, check_vector3_enabled);

}