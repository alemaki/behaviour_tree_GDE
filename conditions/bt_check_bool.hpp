#ifndef BT_CHECK_BOOL_EQUAL_HPP
#define BT_CHECK_BOOL_EQUAL_HPP

#include "behaviour_tree/tasks/bt_condition.hpp"

class BTCheckBool : public BTCondition
{
    GDCLASS(BTCheckBool, BTCondition);

private:
    bool is_true = true;
    godot::StringName var_name;
    bool complain;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    void set_check_true(bool value);
    _FORCE_INLINE_ bool get_check_true() const
    {
        return this->is_true;
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

#endif /* BT_CHECK_BOOL_EQUAL_HPP */