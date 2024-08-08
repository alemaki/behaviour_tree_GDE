#ifndef BT_REPEAT_HPP
#define BT_REPEAT_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTRepeat : public BTDecorator
{
	GDCLASS(BTRepeat, BTDecorator);
private:
    int repeat_times = 1;
    bool abort_on_failure = false;
    bool abort_on_success = false;
    bool run_forever = false;

    int current_repeat_times = 0;

protected:
    virtual void _enter() override;
    virtual BTTask::Status _tick(double delta) override;

public:
    CREATE_GETTER_SETTER_DEFAULT(bool, abort_on_failure);
    CREATE_GETTER_SETTER_DEFAULT(bool, abort_on_success);
    CREATE_GETTER_SETTER_DEFAULT(bool, run_forever);
    CREATE_GETTER_SETTER_POSITIVE_DEFAULT(int, repeat_times);

protected:
    static void _bind_methods();
};

#endif // BT_REPEAT_HPP