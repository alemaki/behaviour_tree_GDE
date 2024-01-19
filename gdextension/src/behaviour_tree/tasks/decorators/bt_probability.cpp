#include "bt_probability.hpp"
#include <godot_cpp/core/math.hpp>

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
    if(godot::Math::randf() < this->run_chance)
    {
        return get_child(0)->execute(delta);
    }
    return BTTask::Status::FAILURE;
}
