#include "hurtbox.hpp"

void Hurtbox::_bind_methods()
{
    using namespace godot;

    ADD_SIGNAL(MethodInfo("hurtbox_hit", PropertyInfo(Variant::OBJECT, "hurtbox")));
}

void Hurtbox::hitbox_entered(godot::Area2D* hitbox)
{
    this->emit_signal("hurtbox_hit", hitbox);
}