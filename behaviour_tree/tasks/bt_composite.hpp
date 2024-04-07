#ifndef BT_COMPOSITE_HPP
#define BT_COMPOSITE_HPP

#include "bt_task.hpp"

class BTComposite : public BTTask
{
	GDCLASS(BTComposite, BTTask);

protected:
	static void _bind_methods(){};
};

#endif // BT_COMPOSITE_HPP