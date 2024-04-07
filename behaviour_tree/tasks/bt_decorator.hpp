#ifndef BT_DECORATOR_HPP
#define BT_DECORATOR_HPP

#include "bt_task.hpp"

class BTDecorator : public BTTask
{
	GDCLASS(BTDecorator, BTTask);

protected:
	static void _bind_methods(){};
};

#endif // BT_DECORATOR_HPP