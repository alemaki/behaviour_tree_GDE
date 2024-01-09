#include "character_input_component.hpp"
#include <godot_cpp/classes/input.hpp>


void CharacterInputComponent::_bind_methods()
{
    using namespace godot;
    
    ClassDB::bind_method(D_METHOD("get_direction"),&CharacterInputComponent::get_direction_input);
}

void CharacterInputComponent::_evaluate_input(const godot::Ref<godot::InputEvent> event)
{
    godot::Input* input = godot::Input::get_singleton();
    this->direction_input = input->get_vector("ui_left", "ui_right", "ui_up", "ui_down");
}

void CharacterInputComponent::_ready()
{
    this->connect("input_event", godot::Callable(this, "_evaluate_input"));
}

godot::Vector2 CharacterInputComponent::get_direction_input() const
{
    return this->direction_input;
}