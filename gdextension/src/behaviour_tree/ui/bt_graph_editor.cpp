#include "bt_graph_editor.hpp"
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

BTGraphEditor::BTGraphEditor()
{
    this->graph_editor = memnew(godot::GraphEdit);
    this->graph_editor->set_h_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);
    this->graph_editor->set_v_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);

    this->graph_editor->connect("connection_request", callable_mp(this, &BTGraphEditor::connection_request));

    this->graph_editor->add_valid_connection_type(1, 0);
}

BTGraphEditor::~BTGraphEditor()
{
    
}

void BTGraphEditor::set_editor_plugin(godot::EditorPlugin* editor_plugin)
{
    this->editor_plugin = editor_plugin;
}

void BTGraphEditor::connection_request(godot::StringName _from_node, int from_port, godot::StringName _to_node, int to_port)
{

    ERR_FAIL_COND_MSG(!(this->node_map.has(_from_node)), "From_node doesn't exist.");
    ERR_FAIL_COND_MSG(!(this->node_map.has(_to_node)), "To_node doesn't exist.");

    BTGraphNode* from_node = this->node_map[_from_node];
    BTGraphNode* to_node = this->node_map[_to_node];

    bool can_connect = this->behaviour_tree->can_connect(from_node->get_task(), to_node->get_task());
    if (can_connect)
    {
        godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

        undo_redo_manager->create_action("Create a connection between nodes.");

        int index = this->get_node_position_in_children(from_node, to_node);

        undo_redo_manager->add_do_method(this->behaviour_tree, "connect_tasks", from_node->get_task(), to_node->get_task(), index);
        undo_redo_manager->add_undo_method(this->behaviour_tree, "disconnect_tasks", from_node->get_task(), to_node->get_task());

        undo_redo_manager->add_do_method(this->graph_editor, "connect_node", from_node->get_name(), from_port, to_node->get_name(), to_port);
        undo_redo_manager->add_undo_method(this->graph_editor, "disconnect_node", from_node->get_name(), from_port, to_node->get_name(), to_port);

        undo_redo_manager->commit_action();
    }
}

int BTGraphEditor::get_node_position_in_children(BTGraphNode* parent_graph_node, BTGraphNode* graph_node)
{
    godot::Ref<BTTask> task = graph_node->get_task();

    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*> task_to_node;
    godot::Array bt_graph_nodes = this->get_graph_nodes();

    for (int i = 0, size = bt_graph_nodes.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(bt_graph_nodes[i]);
        task_to_node[bt_graph_node->get_task()] = bt_graph_node;
    }

    godot::Array all_children = parent_graph_node->get_task()->get_children();
    int size = all_children.size();
    if (size == 0)
    {
        return 0;
    }

    for (int i = 0; i < size - 1; i++)
    {
        BTGraphNode* node1 = task_to_node[all_children[i]];
        BTGraphNode* node2 = task_to_node[all_children[i + 1]];

        if (graph_node->get_position_offset().y >= node1->get_position_offset().y &&
            graph_node->get_position_offset().y <= node2->get_position_offset().y)
        {
            return i + 1;
        }
    }
    BTGraphNode* first_child = task_to_node[all_children[0]];

    if (graph_node->get_position_offset().y < first_child->get_position_offset().y)
    {
        return 0;
    }
    /* Last option is after last*/
    return size;
}

void BTGraphEditor::_node_dragged(const godot::Vector2 &_from, const godot::Vector2 &_to, godot::StringName node_name)
{
    ERR_FAIL_COND_MSG(!(this->node_map.has(node_name)), "Dragged an invalid node");

    godot::UtilityFunctions::print("Dragged node: " + node_name);

    BTGraphNode* dragged_graph_node = this->node_map[node_name];
    godot::Ref<BTTask> dragged_task = dragged_graph_node->get_task();

    if (!(dragged_task->get_parent().is_valid()))
    {
        /* Nothing to do here.*/
        return;
    }

    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*> task_to_node;
    godot::Array bt_graph_nodes = this->get_graph_nodes();

    for (int i = 0, size = bt_graph_nodes.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(bt_graph_nodes[i]);
        task_to_node[bt_graph_node->get_task()] = bt_graph_node;
    }

    godot::Ref<BTTask> parent_task = dragged_task->get_parent();
    BTGraphNode* parent_graph_node = task_to_node[parent_task];

    parent_task->remove_child(dragged_task);
    int index = get_node_position_in_children(parent_graph_node, dragged_graph_node);
    parent_task->add_child_at_index(dragged_task, index);

    return;
}

void BTGraphEditor::clear_graph_nodes()
{
    this->graph_editor->clear_connections();

    godot::TypedArray<godot::Node> children = this->graph_editor->get_children();
    for (int i = 0, size = children.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(children[i]);

        if (bt_graph_node != nullptr)
        {
            this->graph_editor->remove_child(bt_graph_node);
            bt_graph_node->queue_free();
        }
    }
}

void BTGraphEditor::create_default_graph_nodes()
{
    godot::UtilityFunctions::print("Creating default graph nodes.");
    godot::Array tasks = this->behaviour_tree->get_tasks();

    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*> task_to_node;
    for (int i = 0, size = tasks.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = new_bt_graph_node_from_task(godot::Ref<BTTask>(tasks[i]));

        bt_graph_node->set_graph_editor(this->graph_editor);

        int id = this->behaviour_tree->get_task_id(godot::Ref<BTTask>(tasks[i]));
        bt_graph_node->set_title(godot::String("[") + godot::itos(id) + godot::String("]"));
        bt_graph_node->set_name(godot::itos(id));

        this->node_map.insert(godot::itos(id), bt_graph_node);

        this->graph_editor->add_child(bt_graph_node);

        bt_graph_node->connect("dragged", callable_mp(this, &BTGraphEditor::_node_dragged).bind(godot::itos(id)));

        task_to_node.insert(tasks[i], bt_graph_node);
    }
    
    for (int i = 0, size = tasks.size(); i < size; i++)
    {
        BTGraphNode* parent_node = task_to_node[godot::Ref<BTTask>(tasks[i])];
        godot::Array children = parent_node->get_task()->get_children();

        for (int i = 0, size = children.size(); i < size; i++)
        {
            BTGraphNode* child_node = task_to_node[godot::Ref<BTTask>(children[i])];
            this->graph_editor->connect_node(parent_node->get_name(), 0, child_node->get_name(), 0);
        }
    }

    this->arrange_nodes();
}

godot::Array BTGraphEditor::get_graph_nodes()
{
    godot::Array result;
    for (const godot::KeyValue<godot::StringName, BTGraphNode*>& element : this->node_map)
    {
        if (element.value == nullptr)
        {
            godot::UtilityFunctions::printerr("Invalid node from reading. Key: " + element.key);
            continue;
        }
        result.push_back(element.value);
    }
    return result;
}

void BTGraphEditor::_extract_node_levels_into_stack(
    BTGraphNode* root_node, 
    godot::Vector<godot::Pair<BTGraphNode*, int>>& stack, 
    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*>& task_to_node, 
    int current_level)
{
    stack.push_back(godot::Pair<BTGraphNode*, int>(root_node, current_level));

    godot::Array children = root_node->get_task()->get_children();
    /* reverse so the first node to exit is the first child */
    children.reverse(); 
    for (int i = 0, size = children.size(); i < size; i++)
    {
        BTGraphNode* child_node = Object::cast_to<BTGraphNode>(task_to_node[children[i]]);
        this->_extract_node_levels_into_stack(child_node, stack, task_to_node, current_level + 1);
    }
}

void BTGraphEditor::arrange_nodes()
{
    godot::HashMap<godot::Ref<BTTask>, BTGraphNode*> task_to_node;
    godot::Array bt_graph_nodes = this->get_graph_nodes();

    if (bt_graph_nodes.size() == 0)
    {
        return;
    }
    for (int i = 0, size = bt_graph_nodes.size(); i < size; i++)
    {
        BTGraphNode* bt_graph_node = Object::cast_to<BTGraphNode>(bt_graph_nodes[i]);
        task_to_node[bt_graph_node->get_task()] = bt_graph_node;
    }
    BTGraphNode* root_node = task_to_node[this->behaviour_tree->get_root_task()];

    godot::Vector<godot::Pair<BTGraphNode*, int>> stack;
    this->_extract_node_levels_into_stack(root_node, stack, task_to_node);

    godot::HashMap<int, int> level_to_node_count;
    godot::Vector2 root_node_position = root_node->get_position_offset();
    godot::Vector2 node_padding = godot::Vector2(150, 50);
    int current_index = stack.size() - 1;

    while (current_index >= 0)
    {
        godot::Pair<BTGraphNode*, int> pair_node_level = stack[current_index];

        godot::Vector2 position = godot::Vector2(0, 0);
        position.x += node_padding.x*pair_node_level.second;
        position.y += node_padding.y*level_to_node_count[pair_node_level.second];
        position += root_node_position;

        pair_node_level.first->set_position_offset(position);

        level_to_node_count[pair_node_level.second]++;
        current_index--;
    }
}

void BTGraphEditor::set_behaviour_tree(BehaviourTree* new_tree)
{
    this->behaviour_tree = new_tree;
    this->clear_graph_nodes();
    this->create_default_graph_nodes();
}

BTGraphNode* BTGraphEditor::new_bt_graph_node_from_task(godot::Ref<BTTask> bt_task)
{
    BTGraphNode* bt_graph_node = memnew(BTGraphNode);

    bt_graph_node->set_task(bt_task);

    return bt_graph_node;
}

BTGraphNode* BTGraphEditor::new_bt_graph_node()
{
    godot::Ref<BTTask> bt_task = godot::Ref<BTTask>(memnew(BTTask));
    BTGraphNode* bt_graph_node = this->new_bt_graph_node_from_task(bt_task);

    return bt_graph_node;
}

void BTGraphEditor::insert_node(BTGraphNode* bt_graph_node)
{
    this->node_map.insert(bt_graph_node->get_name(), bt_graph_node);
}

void BTGraphEditor::erase_node(BTGraphNode* bt_graph_node)
{
    this->node_map.erase(bt_graph_node->get_name());
}

void BTGraphEditor::_add_new_node_button_pressed()
{
    BTGraphNode* bt_graph_node = this->new_bt_graph_node();

    bt_graph_node->set_graph_editor(this->graph_editor);

    int id = this->behaviour_tree->get_valid_id();
    bt_graph_node->set_title(godot::String("[") + godot::itos(id) + godot::String("]"));
    bt_graph_node->set_name(godot::itos(id));


    bt_graph_node->connect("dragged", callable_mp(this, &BTGraphEditor::_node_dragged).bind(godot::itos(id)));

    /* TODO:
    /* bt_graph_node->connect("node_selected", callable_mp(this, &BTEditorPlugin::_node_selected).bind(id));
    /* bt_graph_node->connect("node_deselected", callable_mp(this, &BTEditorPlugin::_node_deselected).bind(id)); */

    godot::EditorUndoRedoManager* undo_redo_manager = this->editor_plugin->get_undo_redo();

    undo_redo_manager->create_action("Add a node.");

    undo_redo_manager->add_do_method(this->behaviour_tree, "add_task", id, bt_graph_node->get_task());
    undo_redo_manager->add_do_method(this->graph_editor, "add_child", bt_graph_node);
    undo_redo_manager->add_do_method(this, "insert_node", bt_graph_node);

    undo_redo_manager->add_undo_method(this, "erase_node", bt_graph_node);
    undo_redo_manager->add_undo_method(this->graph_editor, "remove_child", bt_graph_node);
    undo_redo_manager->add_undo_method(this->behaviour_tree, "remove_task", id);

    undo_redo_manager->commit_action();

}

void BTGraphEditor::_arrange_nodes_button_pressed()
{
    this->arrange_nodes();
}

void BTGraphEditor::_clear_graph_button_pressed()
{
    this->clear_graph_nodes();
    this->behaviour_tree->clear_tasks();
    this->node_map.clear();
}

void BTGraphEditor::_bind_methods()
{
    using namespace godot;
    ClassDB::bind_method(D_METHOD("insert_node", "bt_graph_node"), &BTGraphEditor::insert_node);
    ClassDB::bind_method(D_METHOD("erase_node", "bt_graph_node"), &BTGraphEditor::erase_node);

    ClassDB::bind_method(D_METHOD("move_nodes"), &BTGraphEditor::move_nodes);
    /* unneeded for now */
    // ClassDB::bind_method(D_METHOD("add_node_method", "id", "bt_graph_node"), &BTGraphEditor::add_node_method);
    // ClassDB::bind_method(D_METHOD("remove_node_method", "id", "bt_graph_node"), &BTGraphEditor::remove_node_method);
    // ClassDB::bind_method(D_METHOD("connect_nodes_method", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::connect_nodes_method);
    // ClassDB::bind_method(D_METHOD("disconnect_nodes_method", "from_node", "from_port", "to_node", "to_port"), &BTGraphEditor::disconnect_nodes_method);

    ClassDB::bind_method(D_METHOD("set_behaviour_tree", "behaviour_tree"), &BTGraphEditor::set_behaviour_tree);
    ClassDB::bind_method(D_METHOD("get_behaviour_tree"), &BTGraphEditor::get_behaviour_tree);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "behavior_tree"), "set_behavior_tree", "get_behavior_tree");

}