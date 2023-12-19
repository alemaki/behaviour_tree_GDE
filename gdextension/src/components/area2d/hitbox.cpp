#include "hitbox.hpp"
#include "components/area2d/hurtbox.hpp"
Hitbox::Hitbox()
{
    this->connect("area_entered", godot::Callable(this, "_on_area_entered"));
}

void Hitbox::_on_area_entered(godot::Area2D* area2d)
{
    Hurtbox* hurtbox = godot::Object::cast_to<Hurtbox>(area2d);
    if (hurtbox != nullptr)
    {
        hurtbox->hitbox_entered(this);
        this->emit_signal("hit_hurtbox", hurtbox);
    }
}

void Hitbox::_bind_methods()
{
    using namespace godot;
    
    //NOTE: you are supposed to bind methods called by signals.
    ClassDB::bind_method(D_METHOD("_on_area_entered", "area2d"), &Hitbox::_on_area_entered);

    ADD_SIGNAL(MethodInfo("hit_hurtbox", PropertyInfo(Variant::OBJECT, "hitbox", PROPERTY_HINT_NONE, "Area2D")));
}