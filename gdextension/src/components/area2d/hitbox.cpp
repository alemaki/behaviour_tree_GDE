#include "hitbox.hpp"
#include "components/area2d/hurtbox.hpp"


void Hitbox::_bind_methods()
{
    using namespace godot;
    
    ADD_SIGNAL(MethodInfo("hit_hurtbox", PropertyInfo(Variant::OBJECT, "hitbox")));
}

void Hitbox::_ready()
{
    this->connect("area_entered", godot::Callable(this, "_on_area_entered"));
}

void Hitbox::_on_area_entered(godot::Area2D* area2d)
{
    Hurtbox* hurtbox = godot::Object::cast_to<Hurtbox>(area2d);
    if (hurtbox != nullptr)
    {
        hurtbox->hitbox_entered(this);
        this->emit_signal("hit_hurtbox", area2d);
    }
}