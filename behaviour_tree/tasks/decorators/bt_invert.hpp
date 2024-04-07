#ifndef BT_INVERT_HPP
#define BT_INVERT_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTInvert : public BTDecorator
{
	GDCLASS(BTInvert, BTDecorator);

protected:
    virtual BTTask::Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_INVERT_HPP