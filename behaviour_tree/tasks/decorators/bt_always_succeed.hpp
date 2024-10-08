#ifndef BT_ALWAYS_SUCCEED_HPP
#define BT_ALWAYS_SUCCEED_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTAlwaysSucceed : public BTDecorator
{
	GDCLASS(BTAlwaysSucceed, BTDecorator);

protected:
    virtual BTTask::Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_ALWAYS_SUCCEED_HPP