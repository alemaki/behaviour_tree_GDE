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

	ClassDB::bind_method(D_METHOD("get_successes_required"), &BTParallel::get_successes_required);
	ClassDB::bind_method(D_METHOD("set_successes_required", "value"), &BTParallel::set_successes_required);
	ClassDB::bind_method(D_METHOD("get_failures_required"), &BTParallel::get_failures_required);
	ClassDB::bind_method(D_METHOD("set_failures_required", "value"), &BTParallel::set_failures_required);
	ClassDB::bind_method(D_METHOD("get_repeat"), &BTParallel::get_repeat);
	ClassDB::bind_method(D_METHOD("set_repeat", "enable"), &BTParallel::set_repeat);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "num_successes_required"), "set_successes_required", "get_successes_required");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "num_failures_required"), "set_failures_required", "get_failures_required");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "repeat"), "set_repeat", "get_repeat");
}