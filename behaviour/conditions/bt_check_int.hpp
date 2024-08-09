#ifndef BT_CHECK_INT_EQUAL_HPP
#define BT_CHECK_INT_EQUAL_HPP

#include "behaviour_tree/tasks/bt_condition.hpp"

class BTCheckInt : public BTCondition
{
    GDCLASS(BTCheckInt, BTCondition);

private:
    int value;
    godot::StringName var_name;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    CREATE_GETTER_SETTER_DEFAULT(int, value);
    CREATE_GETTER_SETTER_STRINGNAME_DEFAULT(var_name);

protected:
    static void _bind_methods();
};

#endif /* BT_CHECK_INT_EQUAL_HPP */