#include "bt_action.hpp"

void BTAction::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(BTAction, complain_enabled);
}
