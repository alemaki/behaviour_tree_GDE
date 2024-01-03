#ifndef CHARACTER_MOVEMENT_COMPONENT
#define CHARACTER_MOVEMENT_COMPONENT

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>

class CharacterMovementComponent : public godot::Node
{
    GDCLASS(CharacterMovementComponent, godot::Node);

private:
    godot::Variant character;
    godot::Variant input_component;

    float speed;
    float friction;

    godot::Vector2 current_velocity;

protected:
    void _physics_process(double delta) override;
    void _ready() override;
    static void _bind_methods();

public:
    void set_character(godot::Variant character);
    godot::Variant get_character() const;
    void set_input_component(godot::Variant input_component);
    godot::Variant get_input_component() const;
    void set_speed(float speed);
    float get_speed() const;
    void set_friction(float friction);
    float get_friction() const;

};

#endif // CHARACTER_MOVEMENT_COMPONENT