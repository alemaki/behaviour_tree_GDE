#ifndef BT_CONDITION_HPP
#define BT_CONDITION_HPP

#include "bt_task.hpp"

class BTCondition : public BTTask
{
	GDCLASS(BTCondition, BTTask);

private:
	bool complain_enabled = true;

public:
	CREATE_GETTER_SETTER_BOOL_DEFAULT(complain_enabled);

protected:
	static void _bind_methods();
};

#endif // BT_CONDITION_HPP