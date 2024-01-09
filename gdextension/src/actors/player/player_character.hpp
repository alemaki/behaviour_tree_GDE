#ifndef PLAYER_CHARACTER_HPP
#define PLAYER_CHARACTER_HPP

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/variant/variant.hpp>

class PlayerCharacter : public godot::CharacterBody2D
{
    GDCLASS(PlayerCharacter, CharacterBody2D);

protected:
    static void _bind_methods();

};

#endif // PLAYER_CHARACTER_HPP