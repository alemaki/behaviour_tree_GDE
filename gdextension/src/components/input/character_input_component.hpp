#ifndef CHARACTER_INPUT_COMPONENT
#define CHARACTER_INPUT_COMPONENT

#include <godot_cpp/classes/node.hpp>

class CharacterInputComponent : public godot::Node
{
    GDCLASS(CharacterInputComponent, godot::Node);

protected:
    godot::Vector2 direction_input;

    void evaluate_input();

public:
    godot::Vector2 get_direction_input() const;
    void _process(double delta) override;

protected:
    static void _bind_methods();

};

#endif // CHARACTER_INPUT_COMPONENT