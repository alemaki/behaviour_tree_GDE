#include "projectile.hpp"


void Projectile::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("_physics_process", "delta"), &Projectile::_physics_process);
}