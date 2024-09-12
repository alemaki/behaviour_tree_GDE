#include "bt_dynamic_selector.hpp"

void BTDynamicSelector::_enter()
{
    this->last_running_index = 0;
}

BTTask::Status BTDynamicSelector::_tick(double delta)
{
    BTTask::Status result = BTTask::Status::SUCCESS;
    int i = 0;
    for (; i < this->get_child_count(); i++)
    {
        result = this->get_child(i)->execute(delta);
        if (result != BTTask::FAILURE)
        {
            break;
        }
    }

    if (this->last_running_index > i && this->get_child(this->last_running_index)->get_status() == BTTask::RUNNING)
    {
        this->get_child(this->last_running_index)->abort();
    }
    this->last_running_index = i;
    return result;
}