#include "bt_graph_node_subtree.hpp"

BTGraphNodeSubtree::BTGraphNodeSubtree() : BTGraphNode()
{
    this->set_slot(0, true, 0, godot::Color::named("WHITE"),
                      false, 1, godot::Color::named("WHITE"));
    

    this->_setup_subtree_labels();

    this->set_self_modulate(godot::Color::named("YELLOW"));
}

void BTGraphNodeSubtree::_setup_subtree_labels()
{
    this->path_label = memnew(godot::Label);
    this->path_label->set_text("");
    this->add_child(this->path_label);
}

void BTGraphNodeSubtree::set_task(godot::Ref<BTTask> task)
{
    godot::Ref<BTSubtree> subtree = godot::Ref<BTSubtree>(task);

    ERR_FAIL_COND_MSG(subtree == nullptr, "Not a subtree.");

    this->task_type_label->set_text(BTSubtree::get_class_static());
    this->task = task;
}


void BTGraphNodeSubtree::set_file_path(const godot::String& path)
{
    godot::Ref<BTSubtree> subtree_task = godot::Ref<BTSubtree>(this->task);
    ERR_FAIL_COND(subtree_task == nullptr);

    this->path_label->set_text(path);

    BehaviourTree* behaviour_tree = subtree_task->load_behavior_tree();
    if (behaviour_tree == nullptr)
    {
        this->set_title("");
        ERR_FAIL_EDMSG("Path is not valid: " + path);
    }

    this->set_title(behaviour_tree->get_name());
    /* TODO: bad structure if I have to free memory here. Fix*/
    memfree(behaviour_tree);
}

void BTGraphNodeSubtree::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_file_path", "path"), &BTGraphNodeSubtree::set_file_path);
    ClassDB::bind_method(D_METHOD("get_file_path"), &BTGraphNodeSubtree::get_file_path);
}