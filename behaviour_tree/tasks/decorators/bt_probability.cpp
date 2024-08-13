#include "bt_probability.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

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

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTProbability, FLOAT, run_chance);
}