#ifndef HITBOX_HPP
#define HITBOX_HPP

#include <godot_cpp/classes/area2d.hpp>

class Hitbox : public godot::Area2D
{
    GDCLASS(Hitbox, godot::Area2D);

protected:
    void _on_area_entered(godot::Ref<godot::Area2D> area2d);

public: 
    void _ready() override;
    static void _bind_methods();
};


#endif