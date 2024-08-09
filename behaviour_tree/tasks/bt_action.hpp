#ifndef BT_ACTION_HPP
#define BT_ACTION_HPP

#include "bt_task.hpp"

class BTAction : public BTTask
{
	GDCLASS(BTAction, BTTask);

private:
	bool complain_enabled = true;

public:
	CREATE_GETTER_SETTER_BOOL_DEFAULT(complain_enabled);

protected:
	static void _bind_methods();
};

#endif // BT_ACTION_HPP