#ifndef BT_ALWAYS_FAIL_HPP
#define BT_ALWAYS_FAIL_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTAlwaysFail : public BTDecorator
{
	GDCLASS(BTAlwaysFail, BTDecorator);

public:
    virtual BTTask::Status _tick(double delta) override;
};

#endif // BT_ALWAYS_FAIL_HPP