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

public:
    void _physics_process(double delta) override;
    void _ready() override;
    
    void set_character(godot::Variant character);
    void set_input_component(godot::Variant input_component);
    void set_speed(float speed);
    void set_friction(float friction);

    _FORCE_INLINE_ godot::Variant get_character() const;
    _FORCE_INLINE_ godot::Variant get_input_component() const;
    _FORCE_INLINE_ float get_speed() const;
    _FORCE_INLINE_ float get_friction() const;

protected:
    static void _bind_methods();
};

#endif // CHARACTER_MOVEMENT_COMPONENT