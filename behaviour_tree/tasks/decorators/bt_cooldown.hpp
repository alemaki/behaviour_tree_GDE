#ifndef BT_COOLDOWN_HPP
#define BT_COOLDOWN_HPP

#include <godot_cpp/classes/scene_tree_timer.hpp>

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTCooldown : public BTDecorator
{
    GDCLASS(BTCooldown, BTDecorator);

    double duration;
    bool trigger_on_failure;
    bool start_cooled;
    bool cooldown_active;

    Ref<SceneTreeTimer> timer;

private:
    void cool();
    void _on_timeout();

protected:
    BTTask::Status _tick(double delta) override;
    void _setup();

public:
    void set_duration(double duration);
    _FORCE_INLINE_ double get_duration() const
    {
        return this->duration;
    }
    void set_trigger_on_failure(bool trigger_on_failure);
    _FORCE_INLINE_ bool is_trigger_on_failure() const
    {
        return this->trigger_on_failure;
    }
    void set_start_cooled(bool start_cooled);
    _FORCE_INLINE_ bool is_start_cooled() const
    {
        return this->start_cooled;
    }
    _FORCE_INLINE_ bool is_cooldown_active() const
    {
        return this->cooldown_active;
    }
    void reset();
    

protected:
    static void _bind_methods();

};

#endif /* BT_COOLDOWN_HPP */