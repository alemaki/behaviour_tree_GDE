#ifndef BT_CONDITION_HPP
#define BT_CONDITION_HPP

#include "bt_task.hpp"

class BTCondition : public BTTask
{
	GDCLASS(BTCondition, BTTask);

protected:
	static void _bind_methods(){};
};

#endif // BT_CONDITION_HPP