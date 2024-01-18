#include "bt_random_selector.hpp"

void BTRandomSelector::_enter()
{
    int count = this->get_child_count();
    this->last_running_index = 0;
    if (this->indicies.size() != count)
    {
        this->indicies.resize(count);
        for (size_t i = 0; i < count; i++)
        {
            indicies[i] = i;
        }
    }
    indicies.shuffle();
    

}
BTTask::Status BTRandomSelector::_tick(double delta)
{
    BTTask::Status status;
    for (int i = last_running_index, child_count = this->get_child_count(); i < child_count; i++)
    {
        status = this->get_child(indicies[i])->execute(delta);
        if (status != BTTask::Status::FAILURE)
        {
            this->last_running_index = i;
            break;
        }
    }
    return status;
}