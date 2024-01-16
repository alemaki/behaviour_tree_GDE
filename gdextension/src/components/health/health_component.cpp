#include "health_component.hpp"
#include <godot_cpp/core/math.hpp>
void HealthComponent::set_max_hp(int max_hp)
{
    max_hp = godot::Math::clamp<int>(max_hp, 0, max_hp);
    this->max_hp = max_hp;
    this->current_hp = godot::Math::clamp<int>(this->current_hp, 0, this->max_hp);
}

int HealthComponent::get_max_hp() const
{
    return this->max_hp;
}

void HealthComponent::set_current_hp(int current_hp)
{
    current_hp = godot::Math::clamp<int>(current_hp, 0, this->max_hp);
    this->current_hp = current_hp;
}

int HealthComponent::get_current_hp() const
{
    return this->current_hp;
}

void HealthComponent::take_damage(int amount)
{
    amount = godot::Math::clamp<int>(amount, 0, amount);
    this->current_hp -= amount;
    if (this->current_hp <= 0)
    {
        this->current_hp = 0;
        this->emit_signal("health_depleted");
    }
}

void HealthComponent::heal(int amount)
{
    amount = godot::Math::clamp<int>(amount, 0, amount);
    this->current_hp += amount;
    if (this->current_hp > this->max_hp)
    {
        this->current_hp = this->max_hp;
    }
}

void HealthComponent::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("take_damage"), &HealthComponent::take_damage);
    ClassDB::bind_method(D_METHOD("heal"), &HealthComponent::heal);


    ClassDB::bind_method(D_METHOD("set_current_hp", "current_hp"), &HealthComponent::set_current_hp);
    ClassDB::bind_method(D_METHOD("set_max_hp", "max_hp"), &HealthComponent::set_max_hp);
    ClassDB::bind_method(D_METHOD("get_current_hp"), &HealthComponent::get_current_hp);
    ClassDB::bind_method(D_METHOD("get_max_hp"), &HealthComponent::get_max_hp);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_hp"), "set_max_hp", "get_max_hp");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "current_hp"), "set_current_hp", "get_current_hp");

    ADD_SIGNAL(MethodInfo("health_depleted"));
}


