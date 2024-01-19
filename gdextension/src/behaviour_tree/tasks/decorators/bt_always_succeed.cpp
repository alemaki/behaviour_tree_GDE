#include "bt_always_succeed.hpp"

BTTask::Status BTAlwaysSucceed::_tick(double delta)
{
    if (this->has_running_child())
    {
        return BTTask::Status::RUNNING;
    }
    return BTTask::Status::SUCCESS;
}