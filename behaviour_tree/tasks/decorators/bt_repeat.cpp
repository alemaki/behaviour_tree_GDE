#include "bt_repeat.hpp"

void BTRepeat::_enter() 
{
    this->current_repeat_times = 0;
}

BTTask::Status BTRepeat::_tick(double delta) 
{
    if (this->get_child_count() == 0)
    {
        return BTTask::Status::FAILURE;
    }

    BTTask::Status status = this->get_child(0)->execute(delta);
    
    if (status == BTTask::Status::RUNNING)
    {
        return BTTask::Status::RUNNING;
    }
    else if (status == BTTask::Status::SUCCESS && this->abort_on_success)
    {
        return BTTask::Status::SUCCESS;
    }
    else if (status == BTTask::Status::FAILURE && this->abort_on_failure)
    {
        return BTTask::Status::FAILURE;
    }
    else if ((!this->run_forever) || (this->current_repeat_times < this->repeat_times))
    {
        if (!this->run_forever)
        {
            this->current_repeat_times++;
        }
        return BTTask::Status::RUNNING;
    }
    else
    {
        return BTTask::Status::SUCCESS;
    }
}

void BTRepeat::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTRepeat, INT, repeat_times);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTRepeat, BOOL, abort_on_failure);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTRepeat, BOOL, abort_on_success);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTRepeat, BOOL, run_forever);
}
