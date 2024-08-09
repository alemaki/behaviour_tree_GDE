#include "bt_condition.hpp"

void BTCondition::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_complain_enabled", "complain_enabled"), &BTCondition::set_complain_enabled);
    ClassDB::bind_method(D_METHOD("is_complain_enabled"), &BTCondition::is_complain_enabled);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "complain_enabled"), "set_complain_enabled", "is_complain_enabled");
}
