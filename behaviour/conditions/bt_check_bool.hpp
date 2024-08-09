#ifndef BT_CHECK_BOOL_EQUAL_HPP
#define BT_CHECK_BOOL_EQUAL_HPP

#include "behaviour_tree/tasks/bt_condition.hpp"

class BTCheckBool : public BTCondition
{
    GDCLASS(BTCheckBool, BTCondition);

private:
    bool is_true = true;
    godot::StringName var_name;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    CREATE_GETTER_SETTER(bool, check_true, is_true);
    CREATE_GETTER_SETTER_STRINGNAME_DEFAULT(var_name);

protected:
    static void _bind_methods();
};

#endif /* BT_CHECK_BOOL_EQUAL_HPP */