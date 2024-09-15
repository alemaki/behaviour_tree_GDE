#include "bt_always_succeed.hpp"

BTTask::Status BTAlwaysSucceed::_tick(double delta)
{
    if ((this->get_child_count() > 0) && (this->get_child(0)->execute(delta) == BTTask::Status::RUNNING))
    {
		return RUNNING;
	}
    return BTTask::Status::SUCCESS;
}