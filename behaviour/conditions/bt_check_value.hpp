#ifndef BT_CHECK_VALUE_HPP
#define BT_CHECK_VALUE_HPP

#include "behaviour_tree/tasks/bt_condition.hpp"

class BTCheckValue : public BTCondition
{
    GDCLASS(BTCheckValue, BTCondition);

private:
    godot::StringName var_name;

    int int_value = 0;
    double float_value = 0;
    godot::String string_value;
    godot::StringName string_name_value;
    godot::Vector2 vector2_value;
    godot::Vector3 vector3_value;

    bool check_true_enabled = false;
    bool check_int_enabled = false;
    bool check_string_enabled = false;
    bool check_string_name_enabled = false;
    bool check_float_enabled = false;
    bool check_vector2_enabled = false;
    bool check_vector3_enabled = false;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    CREATE_GETTER_SETTER_STRINGNAME_DEFAULT(var_name);

    CREATE_GETTER_SETTER_DEFAULT(int, int_value);
    CREATE_GETTER_SETTER_DEFAULT(double, float_value);
    CREATE_GETTER_SETTER_DEFAULT(godot::String, string_value);
    CREATE_GETTER_SETTER_DEFAULT(godot::StringName, string_name_value);
    CREATE_GETTER_SETTER_DEFAULT(godot::Vector2, vector2_value);
    CREATE_GETTER_SETTER_DEFAULT(godot::Vector3, vector3_value);

    CREATE_GETTER_SETTER_BOOL_DEFAULT(check_true_enabled);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(check_int_enabled);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(check_float_enabled);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(check_string_enabled);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(check_string_name_enabled);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(check_vector2_enabled);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(check_vector3_enabled);

protected:
    static void _bind_methods();
};

#endif /* BT_CHECK_VALUE_HPP */