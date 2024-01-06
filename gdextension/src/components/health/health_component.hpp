#ifndef HEALTH_COMPONENT_HPP
#define HEALTH_COMPONENT_HPP


#include <godot_cpp/classes/node.hpp>

class HealthComponent : public godot::Node
{
    GDCLASS(HealthComponent, godot::Node);
    
public:
    int max_hp;
    int current_hp;

public:
    void set_max_hp(int max_hp);
    int get_max_hp() const;
    void set_current_hp(int current_hp);
    int get_current_hp() const;
    void take_damage(int amount);
    

protected:
    static void _bind_methods();
};

#endif