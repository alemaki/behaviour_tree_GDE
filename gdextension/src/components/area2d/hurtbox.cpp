#include "hurtbox.hpp"

void Hurtbox::_bind_methods()
{
    ADD_SIGNAL(godot::MethodInfo("hurtbox_hit", godot::PropertyInfo(godot::Variant::OBJECT, "hurtbox")));
}

void Hurtbox::hitbox_entered(godot::Area2D* hitbox)
{
    this->emit_signal("hurtbox_hit", hitbox);
}