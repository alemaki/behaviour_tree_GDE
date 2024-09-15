#ifndef BT_ALWAYS_RUN_HPP
#define BT_ALWAYS_RUN_HPP

#include "behaviour_tree/tasks/bt_task.hpp"

/* This class is only for testing purposes. */

class BTAlwaysRun : public BTTask
{
	GDCLASS(BTAlwaysRun, BTTask);

protected:
    virtual BTTask::Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_ALWAYS_RUN_HPP