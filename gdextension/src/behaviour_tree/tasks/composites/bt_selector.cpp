#include "bt_selector.hpp"

void BTSelector::_enter()
{
    this->last_running_index = 0;
}

BTTask::Status BTSelector::_tick(double delta)
{
    BTTask::Status status;
    for (int i = last_running_index, child_count = this->get_child_count(); i < child_count; i++)
    {
        status = this->get_child(i)->execute(delta);
        if (status != BTTask::Status::FAILURE)
        {
            this->last_running_index = i;
            break;
        }
    }
    return status;
}
