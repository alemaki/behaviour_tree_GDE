#include "bt_condition.hpp"

void BTCondition::_bind_methods()
{
    using namespace godot;
    
    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTCondition, complain_enabled);
}
