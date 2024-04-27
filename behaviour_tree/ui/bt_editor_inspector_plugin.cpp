#include "bt_editor_inspector_plugin.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

bool BTEditorInspectorPlugin::_can_handle(Object *object) const
{
    godot::UtilityFunctions::print(object);
    return godot::Object::cast_to<BTGraphNode>(object) != nullptr;
}

void BTEditorInspectorPlugin::_parse_begin(godot::Object *object)
{
    BTGraphNode *node = godot::Object::cast_to<BTGraphNode>(object);
    godot::Ref<BTTask> task = node->get_task();

    godot::Ref<BTProbability> task_probability = task;
    if (task_probability.is_valid())
    {
        godot::Label* label = memnew(godot::Label);
        label->set_text("Probability: ");
        add_custom_control(label);
    }
}

void BTEditorInspectorPlugin::_bind_methods()
{

}