#include "bt_cooldown.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

#include "behaviour_tree/utils/macros.hpp"

void BTCooldown::cool()
{
    this->is_cooldown_active = true;
    if (timer.is_valid())
    {
        timer->set_time_left(this->duration);
    }
    else
    {
        this->timer = godot::Object::cast_to<SceneTree>(godot::Engine::get_singleton()->get_main_loop())->create_timer(duration);
        ERR_FAIL_NULL(this->timer);
        this->timer->connect("timeout", callable_mp(this, &BTCooldown::_on_timeout), CONNECT_ONE_SHOT);
    }
}
void BTCooldown::_on_timeout()
{
    this->is_cooldown_active = false;
    this->timer.unref();
}

BTTask::Status BTCooldown::_tick(double delta)
{
    TASK_FAIL_COND_MSG(this->get_child_count() == 0, "BT decorator has no child.");
    TASK_FAIL_COND(this->is_cooldown_active);

    BTTask::Status status = this->get_child(0)->execute(delta);

    if (status == BTTask::Status::SUCCESS || (this->trigger_on_failure && status == BTTask::FAILURE))
    {
        cool();
    }
    return status;
}

void BTCooldown::_setup()
{
    this->is_cooldown_active = false;

    if (this->start_cooled)
    {
        cool();
    }
}

void BTCooldown::set_duration(double duration)
{
    this->duration = duration;
}

void BTCooldown::set_trigger_on_failure(bool trigger_on_failure)
{
    this->trigger_on_failure = trigger_on_failure;
}

void BTCooldown::set_start_cooled(bool start_cooled)
{
    this->start_cooled = start_cooled;
}

void BTCooldown::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_duration", "duration"), &BTCooldown::set_duration);
    ClassDB::bind_method(D_METHOD("get_duration"), &BTCooldown::get_duration);
    ClassDB::bind_method(D_METHOD("set_trigger_on_failure", "trigger_on_failure"), &BTCooldown::set_trigger_on_failure);
    ClassDB::bind_method(D_METHOD("is_trigger_on_failure"), &BTCooldown::is_trigger_on_failure);
    ClassDB::bind_method(D_METHOD("set_start_cooled", "start_cooled"), &BTCooldown::set_start_cooled);
    ClassDB::bind_method(D_METHOD("us_start_cooled"), &BTCooldown::is_start_cooled);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration", "get_duration");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trigger_on_failure"), "set_trigger_on_failure", "is_trigger_on_failure");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "start_cooled"), "set_start_cooled", "is_start_cooled");
}