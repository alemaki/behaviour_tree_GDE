#ifndef LINEAR_PROJECTILE_HPP
#define LINEAR_PROJECTILE_HPP

#include <godot_cpp/variant/vector2.hpp>
#include "projectiles/projectile.hpp"

class LinearProjectile : public Projectile
{
    GDCLASS(LinearProjectile, Projectile);

protected:

    float speed = 0;
    godot::Vector2 direction = godot::Vector2(0, 0);

public:
    virtual  void _physics_process(double delta) override;

    void set_direction(godot::Vector2 direction);
    godot::Vector2 get_direction() const;
    void set_speed(float speed);
    float get_speed() const;

protected:
    static void _bind_methods();
};

#endif // LINEAR_PROJECTILE_HPP