#include "bt_graph_view.hpp"

BTGraphView::BTGraphView()
{
    this->set_h_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);
    this->set_v_size_flags(godot::Control::SizeFlags::SIZE_EXPAND_FILL);

    /* right to left */
    this->add_valid_connection_type(1, 0);
    this->add_valid_left_disconnect_type(0);

    this->set_right_disconnects(true);

    this->set_show_arrange_button(false);
}

BTGraphView::~BTGraphView()
{
    for (const godot::KeyValue<StringName, TaskNameToNode> key_value : this->saved_graphs)
    {
        for (const godot::KeyValue<StringName, BTGraphNode*> name_node : key_value.value)
        {
            memdelete(name_node.value);
        }
    }
}

void BTGraphView::detach_graph_nodes()
{
    for (const godot::KeyValue<godot::StringName, BTGraphNode*> key_value : this->task_name_to_node)
    {
        this->remove_child(key_value.value);

        /* item_rect_changed signal still linger even after removal of the node. */
        godot::TypedArray<godot::Dictionary> connected_signals = key_value.value->get_signal_connection_list("item_rect_changed");
        
        for (int i = 0; i < connected_signals.size(); i++)
        {
            Dictionary connection_info = connected_signals[i];
            Callable target_callable = connection_info["callable"];

            key_value.value->disconnect("item_rect_changed", target_callable);
        }
    }
    this->task_name_to_node.clear();
    this->clear_connections();
}

bool BTGraphView::save_graph(const godot::StringName& name)
{
    ERR_FAIL_COND_V(this->has_saved_graph(name), false);
    this->saved_graphs.insert(name, this->task_name_to_node);
    this->saved_connection_lists.insert(name, this->get_connection_list());
    return true;
}

bool BTGraphView::_load_graph(const godot::StringName& name)
{
    ERR_FAIL_COND_V_MSG(!(this->has_saved_graph(name)), false, "GraphView doesn't have tree saved: " + name + ".");
    ERR_FAIL_COND_V_MSG(!(this->task_name_to_node.is_empty()), false, "GraphView still has nodes that have to be removed.");

    this->task_name_to_node = this->saved_graphs[name];

    for (const godot::KeyValue<godot::StringName, BTGraphNode*> key_value : this->task_name_to_node)
    {
        this->add_child(key_value.value);
    }

    godot::TypedArray<godot::Dictionary> connections = this->saved_connection_lists[name];

    for (int i = 0; i < connections.size(); i++)
    {
        godot::Dictionary connection = connections[i];
        this->connect_node(connection["from_node"], connection["from_port"], connection["to_node"], connection["to_port"]);
    }

    return true;
}

bool BTGraphView::has_task_name(const godot::StringName& task_name) const
{
    return this->task_name_to_node.has(task_name);
}

void BTGraphView::create_subtree_task_node(const godot::StringName& task_name)
{
    ERR_FAIL_COND(task_name.is_empty());
    ERR_FAIL_COND_MSG(this->has_task_name(task_name), "BTGraphView already has task_name named: " + task_name + ".");

    BTGraphNodeSubtree* graph_node = memnew(BTGraphNodeSubtree);
    this->task_name_to_node[task_name] = graph_node;
    this->add_child(graph_node);
    graph_node->set_focus_mode(godot::Control::FocusMode::FOCUS_CLICK);
    graph_node->set_task_class_name(BTSubtree::get_class_static());
}

void BTGraphView::create_task_node(const godot::StringName& task_name, const godot::StringName& task_class_name)
{
    ERR_FAIL_COND(task_name.is_empty());
    ERR_FAIL_COND(task_class_name.is_empty());
    ERR_FAIL_COND_MSG(this->has_task_name(task_name), "BTGraphView already has task_name named: " + task_name + ".");

    BTGraphNode* graph_node = memnew(BTGraphNode);
    this->task_name_to_node[task_name] = graph_node;
    this->add_child(graph_node);
    graph_node->set_task_class_name(task_class_name);
}

void BTGraphView::set_task_node_title(const godot::StringName& task_name, const godot::String& title)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no task_name named: " + task_name + ".");

    BTGraphNode* graph_node = this->task_name_to_node[task_name];
    graph_node->set_title(title);
}

void BTGraphView::delete_task_node(const godot::StringName& task_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no task_name named: " + task_name + ".");

    BTGraphNode* graph_node = this->task_name_to_node[task_name];

    godot::TypedArray<godot::Dictionary> connection_list = this->get_connection_list();

    for (int i = 0; i < connection_list.size(); i++) {
        const godot::Dictionary& connection = connection_list[i];
        if (
            godot::StringName(connection["from_node"]) == graph_node->get_name() ||
            godot::StringName(connection["to_node"]) == graph_node->get_name())
        {
            this->disconnect_node(connection["from_node"], connection["from_port"], connection["to_node"], connection["to_port"]);
        }
    }

    this->remove_child(graph_node);
    this->task_name_to_node.erase(task_name);
    memdelete(graph_node);
}

void BTGraphView::change_task_class_name(const godot::StringName& task_name, const godot::StringName& task_class_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no task_name named: " + task_name + ".");

    BTGraphNode* graph_node = this->task_name_to_node[task_name];
    graph_node->set_task_class_name(task_class_name);
}

godot::StringName BTGraphView::get_task_name(const godot::StringName& graph_node_name)
{
    for (const godot::KeyValue<godot::StringName, BTGraphNode*>& key_value : this->task_name_to_node)
    {
        if (key_value.value->get_name() == graph_node_name)
        {
            return key_value.key;
        }
    }
    ERR_FAIL_V_MSG("", "Cannot find graph node name: " + graph_node_name);
}

void BTGraphView::connect_task_nodes(const godot::StringName& parent_task_name, const godot::StringName& child_task_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(parent_task_name)), "BTGraphView has no parent task_name named: " + parent_task_name + ".");
    ERR_FAIL_COND_MSG(!(this->has_task_name(child_task_name)), "BTGraphView has no child task_name named: " + child_task_name + ".");
    
    godot::StringName parent_node_name = this->task_name_to_node[parent_task_name]->get_name();
    godot::StringName child_node_name = this->task_name_to_node[child_task_name]->get_name();

    // TODO: fix port numbers?
    this->connect_node(parent_node_name, 0, child_node_name, 0);
}

void BTGraphView::disconnect_task_nodes(const godot::StringName& parent_task_name, const godot::StringName& child_task_name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(parent_task_name)), "BTGraphView has no parent task_name named: " + parent_task_name + ".");
    ERR_FAIL_COND_MSG(!(this->has_task_name(child_task_name)), "BTGraphView has no child task_name named: " + child_task_name + ".");
    
    godot::StringName parent_node_name = this->task_name_to_node[parent_task_name]->get_name();
    godot::StringName child_node_name = this->task_name_to_node[child_task_name]->get_name();

    // TODO: fix port numbers?
    this->disconnect_node(parent_node_name, 0, child_node_name, 0);
}

void BTGraphView::set_node_position(const godot::StringName& task_name, godot::Vector2 position)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(task_name)), "BTGraphView has no task_name named: " + task_name + ".");

    BTGraphNode* node = this->task_name_to_node[task_name];
    node->set_position_offset(position);
}

struct GraphNodeComparatorByY {
    BTGraphView *graph_view;

    bool operator()(const StringName &a, const StringName &b) const {
        return graph_view->get_graph_node(a)->get_position_offset().y < graph_view->get_graph_node(b)->get_position_offset().y;
    }
};

godot::Vector<StringName> BTGraphView::sorted_task_names_by_y(const godot::Vector<StringName>& task_names)
{
    
    for (const godot::StringName& task_name : task_names)
    {
        ERR_FAIL_COND_V_MSG(!(this->has_task_name(task_name)), {}, "BTGraphView has no task_name named: " + task_name + ".");
    }

    godot::Vector<StringName> temp = task_names;
    GraphNodeComparatorByY comparator{this};
    temp.sort_custom<GraphNodeComparatorByY>(comparator);

    return temp;
}

int BTGraphView::find_insert_index_by_y(const godot::StringName& task_name, const godot::Vector<StringName>& task_names)
{
    godot::Vector<StringName> temp = task_names;
    temp.push_back(task_name);
    temp = this->sorted_task_names_by_y(temp);
    return temp.find(task_name);
}

godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> BTGraphView::get_node_tree_map(const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const
{
    godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;

    for (const godot::KeyValue<StringName, godot::Vector<StringName>>& key_value : parent_to_children_names)
    {
        ERR_FAIL_COND_V_MSG(!(this->has_task_name(key_value.key)), {}, "BTGraphView has no task_name named: " + key_value.key + ".");
        BTGraphNode* parent = this->task_name_to_node[key_value.key];
        parent_to_children.insert(parent, {});
        
        for (const godot::StringName& child_name: key_value.value)
        {
            ERR_FAIL_COND_V_MSG(!(this->has_task_name(child_name)), {}, "BTGraphView has no task_name named: " + child_name + ".");
            parent_to_children[parent].push_back(this->task_name_to_node[child_name]);
        }
    }
    return parent_to_children;
}

godot::HashMap<BTGraphNode*, godot::Vector2> BTGraphView::get_arranged_nodes_position(
    const godot::StringName& root_task_name,
    const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names) const
{
    ERR_FAIL_COND_V_MSG(!(this->has_task_name(root_task_name)), false, "BTGraphView has no task_name named: " + root_task_name + ".");

    BTGraphNode* root = this->task_name_to_node[root_task_name];
    godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children = this->get_node_tree_map(parent_to_children_names);

    godot::Ref<BTGraphSortAlgorithm> algorithm = memnew(BTGraphSortAlgorithm);

    algorithm->set_root_node(root);
    algorithm->set_parent_to_children(parent_to_children);

    return algorithm->get_arranged_nodes_position();
}


void BTGraphView::arrange_nodes(
    const godot::StringName& root_task_name,
    const godot::HashMap<StringName, godot::Vector<StringName>>& parent_to_children_names)
{
    godot::HashMap<BTGraphNode*, godot::Vector2> positons = this->get_arranged_nodes_position(root_task_name, parent_to_children_names);

    for (const godot::KeyValue<BTGraphNode*, godot::Vector2>& key_value: positons)
    {
        key_value.key->set_position_offset(key_value.value);
    }
}

bool BTGraphView::has_saved_graph(const godot::StringName& name) const
{
    return this->saved_graphs.has(name) || this->saved_connection_lists.has(name);
}

void BTGraphView::clear_and_save_graph(const godot::StringName& name)
{
    if (!(this->save_graph(name)))
    {
        return;
    }
    this->detach_graph_nodes();
}

void BTGraphView::load_graph(const godot::StringName& name)
{
    if (!(this->_load_graph(name)))
    {
        return;
    }
    this->saved_graphs.erase(name);
    this->saved_connection_lists.erase(name);
}

void BTGraphView::set_root_task_name(const godot::StringName& name)
{
    ERR_FAIL_COND_MSG(!(this->has_task_name(name)), "BTGraphView has no task_name named: " + name + ".");
    if (!(this->root_task_name.is_empty()))
    {
        BTGraphNode* node = this->task_name_to_node[this->root_task_name];
        node->set_default_node_color();
    }

    BTGraphNode* new_root = this->task_name_to_node[name];
    new_root->set_self_modulate(godot::Color::named("BLUE"));
    this->root_task_name = name;
}

void BTGraphView::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("create_task_node", "task_name", "class_name"), &BTGraphView::create_task_node, DEFVAL(BTTask::get_class_static()));
    ClassDB::bind_method(D_METHOD("create_subtree_task_node", "task_name"), &BTGraphView::create_subtree_task_node);
    ClassDB::bind_method(D_METHOD("delete_task_node", "task_name"), &BTGraphView::delete_task_node);
    ClassDB::bind_method(D_METHOD("set_task_node_title", "task_name", "title"), &BTGraphView::set_task_node_title);
    ClassDB::bind_method(D_METHOD("connect_task_nodes", "parent_task_name", "child_task_name"), &BTGraphView::connect_task_nodes);
    ClassDB::bind_method(D_METHOD("disconnect_task_nodes", "parent_task_name", "child_task_name"), &BTGraphView::disconnect_task_nodes);
    ClassDB::bind_method(D_METHOD("set_node_position", "task_name", "position"), &BTGraphView::set_node_position);
}