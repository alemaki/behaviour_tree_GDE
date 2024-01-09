#include "linear_projectile.hpp"

void LinearProjectile::_bind_methods()
{
    using namespace godot;
    ClassDB::bind_method(D_METHOD("set_speed"), &LinearProjectile::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &LinearProjectile::get_speed);
    ClassDB::bind_method(D_METHOD("set_direction"), &LinearProjectile::set_direction);
    ClassDB::bind_method(D_METHOD("get_direction"), &LinearProjectile::get_direction);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "direction"), "set_direction", "get_direction");
}

void LinearProjectile::set_direction(godot::Vector2 direction)
{
    this->direction = direction.normalized();
}

godot::Vector2 LinearProjectile::get_direction() const
{
    return this->direction;
}

void LinearProjectile::set_speed(float speed)
{
    speed = godot::Math::clamp<float>(speed, 0, speed);
    this->speed = speed;
}

float LinearProjectile::get_speed() const
{
    return this->speed;
}

void LinearProjectile::_physics_process(double delta)
{
    Projectile::_physics_process(delta);
    godot::Vector2 vector_travelled = this->direction * this->speed * delta;
    this->set_position(this->get_position() + vector_travelled);
}
