#ifndef HURTBOX_HPP
#define HURTBOX_HPP

#include <godot_cpp/classes/area2d.hpp>


class Hurtbox : public godot::Area2D
{
    GDCLASS(Hurtbox, godot::Area2D);

public:
    void hitbox_entered(godot::Area2D* hitbox);

protected:
    static void _bind_methods();

};


#endif // HURTBOX_HPP