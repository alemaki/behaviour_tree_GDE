#include "bt_always_run.hpp"

BTTask::Status BTAlwaysRun::_tick(double delta)
{
    return BTTask::Status::RUNNING;
}