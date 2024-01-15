#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <godot_cpp/classes/node2d.hpp>

class Projectile : public godot::Node2D
{
    GDCLASS(Projectile, godot::Node2D);

public:
    virtual void _physics_process(double delta) override {};

protected:
    static void _bind_methods();
};

#endif // PROJECTILE_HPP