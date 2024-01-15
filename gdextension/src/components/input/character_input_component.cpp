#include "character_input_component.hpp"
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
void CharacterInputComponent::evaluate_input()
{
    godot::Input* input = godot::Input::get_singleton();
    this->direction_input = input->get_vector("ui_left", "ui_right", "ui_up", "ui_down");
}

void CharacterInputComponent::_process(double delta)
{
    this->evaluate_input();
}
godot::Vector2 CharacterInputComponent::get_direction_input() const
{
    return this->direction_input;
}

void CharacterInputComponent::_bind_methods()
{
    using namespace godot;
    
    ClassDB::bind_method(D_METHOD("get_direction_input"),&CharacterInputComponent::get_direction_input);

    ClassDB::bind_method(D_METHOD("_process", "delta"), &CharacterInputComponent::_process);
}