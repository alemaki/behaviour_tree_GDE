#include "bt_invert.hpp"

BTTask::Status BTInvert::_tick(double delta)
{
     if (this->get_child_count() < 0)
    {
        return BTTask::Status::FAILURE;
    }
    BTTask::Status status = this->get_child(0)->execute(delta);
    if (status == BTTask::Status::SUCCESS)
    {
        return BTTask::Status::FAILURE;
    }
    if (status == BTTask::Status::FAILURE)
    {
        return BTTask::Status::SUCCESS;
    }
    return status;
}