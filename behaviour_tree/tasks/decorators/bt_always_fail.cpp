#include "bt_always_fail.hpp"

BTTask::Status BTAlwaysFail::_tick(double delta)
{
    if ((this->get_child_count() > 0) && (this->get_child(0)->execute(delta) == BTTask::Status::RUNNING))
    {
		return RUNNING;
	}
    return BTTask::Status::FAILURE;
}