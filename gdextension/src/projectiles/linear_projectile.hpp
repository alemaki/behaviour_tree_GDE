#ifndef LINEAR_PROJECTILE_HPP
#define LINEAR_PROJECTILE_HPP

#include <godot_cpp/variant/vector2.hpp>
#include "projectiles/projectile.hpp"

class LinearProjectile : public Projectile
{
    GDCLASS(LinearProjectile, Projectile);

protected:

    float speed;
    godot::Vector2 direction;

protected:

    void _physics_process(double delta) override;
    static void _bind_methods();

public:

    void set_direction(godot::Vector2 direction);
    godot::Vector2 get_direction() const;
    void set_speed(float speed);
    float get_speed() const;
};

#endif // LINEAR_PROJECTILE_HPP