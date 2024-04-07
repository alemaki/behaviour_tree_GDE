#ifndef BT_ALWAYS_FAIL_HPP
#define BT_ALWAYS_FAIL_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTAlwaysFail : public BTDecorator
{
	GDCLASS(BTAlwaysFail, BTDecorator);

protected:
    virtual BTTask::Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_ALWAYS_FAIL_HPP