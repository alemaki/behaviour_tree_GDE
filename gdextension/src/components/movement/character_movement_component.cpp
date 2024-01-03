#include "character_movement_component.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include "components/input/character_input_component.hpp"

void CharacterMovementComponent::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_character", "character"), &CharacterMovementComponent::set_character);
    godot::ClassDB::bind_method(godot::D_METHOD("get_character"), &CharacterMovementComponent::get_character);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "character"), "set_character", "get_character");

    godot::ClassDB::bind_method(godot::D_METHOD("set_input_component", "input_component"), &CharacterMovementComponent::set_input_component);
    godot::ClassDB::bind_method(godot::D_METHOD("get_input_component"), &CharacterMovementComponent::get_input_component);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "input_component"), "set_input_component", "get_input_component");
}

void CharacterMovementComponent::_physics_process(double delta)
{
    CharacterInputComponent* input_component = godot::Object::cast_to<CharacterInputComponent>(this->input_component);
    godot::CharacterBody2D* character = godot::Object::cast_to<godot::CharacterBody2D>(this->character);
    godot::Vector2 target_velocity = godot::Vector2(0, 0);

    if (input_component != nullptr)
    {
        target_velocity = input_component->direction_input * speed;
        
    }

    this->current_velocity += (target_velocity - this->current_velocity) * friction;
    
    if (character != nullptr)
    {
        character->set_velocity(this->current_velocity);
        character->move_and_slide();
    }
}

void CharacterMovementComponent::_ready()
{
    this->current_velocity = godot::Vector2(0, 0);

    godot::CharacterBody2D* character = godot::Object::cast_to<godot::CharacterBody2D>(this->character);
    if (character == nullptr)
    {
        godot::UtilityFunctions::printerr("[CharacterMovementComponent] " + this->get_name() + ": The character isn't set properly.");
    }
    CharacterInputComponent* input_component = godot::Object::cast_to<CharacterInputComponent>(this->input_component);
    if (input_component == nullptr)
    {
        godot::UtilityFunctions::printerr("[CharacterMovementComponent] " + this->get_name() + ": The input component isn't set properly.");
    }
    
}

void CharacterMovementComponent::set_character(godot::Variant character)
{
    this->character = character;
}

godot::Variant CharacterMovementComponent::get_character() const
{
    return this->character;
}

void CharacterMovementComponent::set_input_component(godot::Variant input_component)
{
    this->input_component = input_component;
}

godot::Variant CharacterMovementComponent::get_input_component() const
{
    return this->input_component;
}

void CharacterMovementComponent::set_speed(float speed)
{
    speed = godot::Math::clamp<float>(speed, 0, speed);
    this->speed = speed;
}

float CharacterMovementComponent::get_speed() const
{
    return this->speed;
}

void CharacterMovementComponent::set_friction(float friction)
{
    friction = godot::Math::clamp<float>(friction, 0, friction);
    this->friction = friction;
}

float CharacterMovementComponent::get_friction() const
{
    return this->friction;
}