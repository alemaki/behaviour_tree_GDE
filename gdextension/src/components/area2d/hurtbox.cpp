#include "hurtbox.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

void Hurtbox::hitbox_entered(godot::Area2D* hitbox)
{
    godot::UtilityFunctions::print("ON_HITBOX_ENTERED HURTBOX");
    this->emit_signal("hurtbox_hit", hitbox);
}

void Hurtbox::_bind_methods()
{
    using namespace godot;

    ADD_SIGNAL(MethodInfo("hurtbox_hit", PropertyInfo(Variant::OBJECT, "hurtbox", PROPERTY_HINT_NONE, "Area2D")));
}