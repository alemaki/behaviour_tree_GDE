#ifndef BT_CHECK_INT_EQUAL_HPP
#define BT_CHECK_INT_EQUAL_HPP

#include "behaviour_tree/tasks/bt_condition.hpp"

class BTCheckInt : public BTCondition
{
    GDCLASS(BTCheckInt, BTCondition);

private:
    int value;
    godot::StringName var_name;
    bool complain = true;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    void set_value(int value);
    _FORCE_INLINE_ int get_value() const
    {
        return this->value;
    }
    void set_var_name(const godot::StringName& var_name);
    _FORCE_INLINE_ godot::StringName get_var_name() const
    {
        return this->var_name;
    }
    void set_complain(bool complain);
    _FORCE_INLINE_ bool get_complain() const
    {
        return this->complain;
    }

protected:
    static void _bind_methods();
};

#endif /* BT_CHECK_INT_EQUAL_HPP */