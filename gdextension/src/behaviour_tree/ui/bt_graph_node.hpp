#ifndef BT_GRAPH_NODE_HPP
#define BT_GRAPH_NODE_HPP

//#include <godot_cpp/classes/button.hpp>
//#include <godot_cpp/classes/split_container.hpp>
//#include <godot_cpp/classes/editor_plugin.hpp>

#include <godot_cpp/classes/graph_edit.hpp>
#include <godot_cpp/classes/graph_node.hpp>

#include "behaviour_tree/behaviour_tree.hpp"

class BTGraphNode : public godot::GraphNode
{
    GDCLASS(BTGraphNode, godot::GraphNode)

private:
    godot::GraphEdit* graph_editor;
    godot::Ref<BTTask> task;
    
private:
    void setup_default();
public:
    BTGraphNode();
    void set_graph_editor(godot::GraphEdit* graph_editor);
    _FORCE_INLINE_ godot::GraphEdit* get_graph_editor() const
    {
        return this->graph_editor;
    }

    void set_task(godot::Ref<BTTask> task);

    _FORCE_INLINE_ godot::Ref<BTTask> get_task() const
    {
        return this->task;
    }

protected:
    static void _bind_methods();
};

#endif //BT_GRAPH_NODE_HPP