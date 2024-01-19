#include "bt_always_fail.hpp"

BTTask::Status BTAlwaysFail::_tick(double delta)
{
    if (this->has_running_child())
    {
        return BTTask::Status::RUNNING;
    }
    return BTTask::Status::FAILURE;
}