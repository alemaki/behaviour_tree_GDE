#include "bt_delay.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

void BTDelay::start_delay()
{
    if (this->delay_timer.is_valid())
    {
        this->delay_timer->set_time_left(this->seconds);
    }
    else
    {
        this->delay_timer = godot::Object::cast_to<SceneTree>(godot::Engine::get_singleton()->get_main_loop())->create_timer(this->seconds);
        ERR_FAIL_NULL(this->delay_timer);
        this->delay_timer->connect("timeout", callable_mp(this, &BTDelay::_on_timeout), CONNECT_ONE_SHOT);
    }
    this->delay_active = true;
}

void BTDelay::_on_timeout()
{
    this->execute_child = true;
    this->delay_active = false;
}

void BTDelay::_enter()
{
    this->execute_child = false;
    this->delay_active = false;
}

BTTask::Status BTDelay::_tick(double delta)
{
	TASK_FAIL_COND_MSG(get_child_count() == 0, "BT decorator has no child.");
    if (!(this->execute_child) && !(this->delay_active))
    {
        start_delay();
        return BTTask::RUNNING;
    }
    else if (this->delay_active)
    {
        return BTTask::RUNNING;
    }
    else if (this->execute_child)
    {
        return this->get_child(0)->execute(delta);
    }
    /* Shouldn't ever reach this */
    TASK_FAIL();
}

void BTDelay::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("_on_timeout"), &BTDelay::_on_timeout);
    ClassDB::bind_method(D_METHOD("is_delay_active"), &BTDelay::is_delay_active);

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTDelay, FLOAT, seconds);
}