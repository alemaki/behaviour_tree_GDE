#include "bt_parallel.hpp"

void BTParallel::_enter()
{
    for (int i = 0; i < get_child_count(); i++)
    {
		get_child(i)->abort();
	}
}

BTTask::Status BTParallel::_tick(double delta)
{
    int successes = 0;
    int failures = 0;
    BTTask::Status result = BTTask::Status::RUNNING;
    for (int i = 0; i < get_child_count(); i++)
    {
        BTTask::Status status = BTTask::FRESH;
        Ref<BTTask> child = get_child(i);
        if ((!this->repeat) && ((child->get_status() == FAILURE) || (child->get_status() == SUCCESS)))
        {
            status = child->get_status();
        }
        else
        {
            status = child->execute(delta);
        }
        if (status == BTTask::Status::FAILURE)
        {
            failures++;
            if (failures >= this->failures_required && result == BTTask::Status::RUNNING)
            {
                result = BTTask::Status::FAILURE;
            }
        }
        else if (status == BTTask::Status::SUCCESS)
        {
            successes++;
            if (successes >= this->successes_required && result == BTTask::Status::RUNNING)
            {
                result = BTTask::Status::SUCCESS;
            }
        }
    }
    if ((!this->repeat) && ((successes + failures) == get_child_count()) && result == BTTask::Status::RUNNING)
    {
        result = BTTask::Status::FAILURE;
    }
    return result;
}

void BTParallel::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTParallel, INT, successes_required);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTParallel, INT, failures_required);
    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTParallel, BOOL, repeat);
}