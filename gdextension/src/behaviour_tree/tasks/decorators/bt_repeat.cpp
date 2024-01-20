#include "bt_repeat.hpp"

void BTRepeat::set_repeat_times(int repeat_times)
{
    this->repeat_times = repeat_times;
}

int BTRepeat::get_repeat_times() const
{
    return this->repeat_times;
}

void BTRepeat::set_abort_on_failure(bool abort_on_failure)
{
    this->abort_on_failure = abort_on_failure;
}
bool BTRepeat::get_abort_on_failure() const
{
    return this->abort_on_failure;
}

void BTRepeat::set_abort_on_success(bool abort_on_success)
{
    this->abort_on_success = abort_on_success;
}

bool BTRepeat::get_abort_on_success() const
{
    return this->abort_on_success;
}

void  BTRepeat::set_run_forever(bool run_forever)
{
    this->run_forever = run_forever;
}

bool BTRepeat::get_run_forever() const
{
    return this->run_forever;
}

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

    ClassDB::bind_method(D_METHOD("set_repeat_times", "repeat_times"), &BTRepeat::set_repeat_times);
    ClassDB::bind_method(D_METHOD("get_repeat_times"), &BTRepeat::get_repeat_times);
    ClassDB::bind_method(D_METHOD("set_abort_on_failure", "abort_on_failure"), &BTRepeat::set_abort_on_failure);
    ClassDB::bind_method(D_METHOD("get_abort_on_failure"), &BTRepeat::get_abort_on_failure);
    ClassDB::bind_method(D_METHOD("set_abort_on_success", "abort_on_success"), &BTRepeat::set_abort_on_success);
    ClassDB::bind_method(D_METHOD("get_abort_on_success"), &BTRepeat::get_abort_on_success);
    ClassDB::bind_method(D_METHOD("set_run_forever", "run_forever"), &BTRepeat::set_run_forever);
    ClassDB::bind_method(D_METHOD("get_run_forever"), &BTRepeat::get_run_forever);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "repeat_times"), "set_repeat_times", "get_repeat_times");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "abort_on_failure"), "set_abort_on_failure", "get_abort_on_failure");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "abort_on_success"), "set_abort_on_success", "get_abort_on_success");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "run_forever"), "set_run_forever", "get_run_forever");
}
