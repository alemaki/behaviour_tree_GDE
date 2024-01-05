#ifndef HEALTH_COMPONENT_HPP
#define HEALTH_COMPONENT_HPP


#include <godot_cpp/classes/Node.hpp>
#include <iostream>
class HealthComponent : public godot::Node
{
    GDCLASS(HealthComponent, godot::Node);
    
public:
    int max_hp;
    int current_hp;

public:
    HealthComponent() : max_hp(0), current_hp(0)
    {
        std::cout<<"alabala2"<<std::endl;
        std::cout<<"alabala2"<<std::endl;
        std::cout<<"alabala2"<<std::endl;
        std::cout<<"alabala2"<<std::endl;
    }
    void set_max_hp(int max_hp);
    int get_max_hp() const;
    void set_current_hp(int current_hp);
    int get_current_hp() const;
    void take_damage(int amount);
    

protected:
    static void _bind_methods();
};

#endif