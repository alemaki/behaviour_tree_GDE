#include "bt_cooldown.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

#include "behaviour_tree/utils/macros.hpp"

void BTCooldown::cool()
{
    this->cooldown_active = true;
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
    this->cooldown_active = false;
    this->timer.unref();
}

BTTask::Status BTCooldown::_tick(double delta)
{
    TASK_FAIL_COND_MSG(this->get_child_count() == 0, "BT decorator has no child.");
    TASK_FAIL_COND(this->cooldown_active);

    BTTask::Status status = this->get_child(0)->execute(delta);

    if (status == BTTask::Status::SUCCESS || (this->trigger_on_failure && status == BTTask::FAILURE))
    {
        cool();
    }
    return status;
}

void BTCooldown::_setup()
{
    this->cooldown_active = false;

    if (this->start_cooled)
    {
        cool();
    }
}

void BTCooldown::reset()
{
    this->_on_timeout();
}

void BTCooldown::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("_on_timeout"), &BTCooldown::_on_timeout);
    ClassDB::bind_method(D_METHOD("is_cooldown_active"), &BTCooldown::is_cooldown_active);

    ClassDB::bind_method(D_METHOD("set_duration", "duration"), &BTCooldown::set_duration);
    ClassDB::bind_method(D_METHOD("get_duration"), &BTCooldown::get_duration);
    ClassDB::bind_method(D_METHOD("set_trigger_on_failure", "trigger_on_failure"), &BTCooldown::set_trigger_on_failure);
    ClassDB::bind_method(D_METHOD("is_trigger_on_failure"), &BTCooldown::is_trigger_on_failure);
    ClassDB::bind_method(D_METHOD("set_start_cooled", "start_cooled"), &BTCooldown::set_start_cooled);
    ClassDB::bind_method(D_METHOD("is_start_cooled"), &BTCooldown::is_start_cooled);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration", "get_duration");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trigger_on_failure"), "set_trigger_on_failure", "is_trigger_on_failure");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "start_cooled"), "set_start_cooled", "is_start_cooled");
}