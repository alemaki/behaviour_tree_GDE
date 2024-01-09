#ifndef CHARACTER_INPUT_COMPONENT
#define CHARACTER_INPUT_COMPONENT

#include <godot_cpp/classes/node.hpp>

class CharacterInputComponent : public godot::Node
{
    GDCLASS(CharacterInputComponent, godot::Node);

protected:
    godot::Vector2 direction_input;

    void _evaluate_input(const godot::Ref<godot::InputEvent> event);

public:
    void _ready() override;
    
    _FORCE_INLINE_ godot::Vector2 get_direction_input() const;

protected:
    static void _bind_methods();
};

#endif // CHARACTER_INPUT_COMPONENT