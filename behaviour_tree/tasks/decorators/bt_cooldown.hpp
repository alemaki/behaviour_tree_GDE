#ifndef BT_COOLDOWN_HPP
#define BT_COOLDOWN_HPP

#include <godot_cpp/classes/scene_tree_timer.hpp>

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTCooldown : public BTDecorator
{
    GDCLASS(BTCooldown, BTDecorator);

    double duration = 0;
    bool trigger_on_failure = false;
    bool start_cooled = false;
    bool cooldown_active = false;

    Ref<SceneTreeTimer> timer;

private:
    void cool();
    void _on_timeout();

protected:
    BTTask::Status _tick(double delta) override;
    void _setup();

public:
    CREATE_GETTER_SETTER_POSITIVE_DEFAULT(double, duration);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(trigger_on_failure);
    CREATE_GETTER_SETTER_BOOL_DEFAULT(start_cooled);
    
    _FORCE_INLINE_ bool is_cooldown_active() const
    {
        return this->cooldown_active;
    }
    void reset();
    

protected:
    static void _bind_methods();

};

#endif /* BT_COOLDOWN_HPP */