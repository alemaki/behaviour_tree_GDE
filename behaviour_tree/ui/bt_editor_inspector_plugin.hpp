#ifndef BT_INSPECTOR_PLUGIN_HPP
#define BT_INSPECTOR_PLUGIN_HPP

#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/label.hpp>

#include "behaviour_tree/ui/bt_graph_node.hpp"

class BTEditorInspectorPlugin : public godot::EditorInspectorPlugin
{
    GDCLASS(BTEditorInspectorPlugin, godot::EditorInspectorPlugin)

public:
    bool _can_handle(Object *object) const override;
    void _parse_begin(Object *object) override;

protected:
    static void _bind_methods();
};

#endif //BT_INSPECTOR_PLUGIN_HPP