#ifndef BT_INVERT_HPP
#define BT_INVERT_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTInvert : public BTDecorator
{
	GDCLASS(BTInvert, BTDecorator);

public:
    virtual BTTask::Status _tick(double delta) override;
};

#endif // BT_INVERT_HPP