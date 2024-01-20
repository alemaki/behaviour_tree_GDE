#include "bt_probability.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

void BTProbability::set_run_chance(float chance)
{
    chance = godot::Math::clamp<float>(chance, 0, 1);
    this->run_chance = chance;
}
float BTProbability::get_run_chance() const
{
    return this->run_chance;
}
BTTask::Status BTProbability::_tick(double delta)
{
    if(godot::UtilityFunctions::randf() < this->run_chance)
    {
        return get_child(0)->execute(delta);
    }
    return BTTask::Status::FAILURE;
}

void BTProbability::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_run_chance", "chance"), &BTProbability::set_run_chance);
    ClassDB::bind_method(D_METHOD("get_run_chance"), &BTProbability::get_run_chance);
    
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "run_chance"), "set_run_chance", "get_run_chance");
}