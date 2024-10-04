#include "bt_graph_node_subtree.hpp"
#include "behaviour_tree/utils/utils.hpp"

BTGraphNodeSubtree::BTGraphNodeSubtree() : BTGraphNode()
{
    this->set_slot(0, true, 0, godot::Color::named("WHITE"),
                      false, 1, godot::Color::named("WHITE"));
    

    this->_setup_subtree_labels();

    this->set_self_modulate(godot::Color::named("YELLOW"));
    this->set_task_class_name(BTSubtree::get_class_static());
}

void BTGraphNodeSubtree::_setup_subtree_labels()
{
    this->path_label = memnew(godot::Label);
    this->path_label->set_text("");
    this->add_child(this->path_label);
}

void BTGraphNodeSubtree::set_file_path(const godot::String& path)
{
    this->path = path;
    this->path_label->set_text(path);

    BehaviourTree* behaviour_tree = utils::load_scene_node<BehaviourTree>(this->path);
    if (behaviour_tree == nullptr)
    {
        this->set_title("");
        ERR_FAIL_EDMSG("Path is not valid: " + path);
    }

    this->set_title(behaviour_tree->get_name());
    /* TODO: bad structure if I have to free memory here. Fix*/
    memdelete(behaviour_tree);
}

void BTGraphNodeSubtree::_bind_methods()
{
    using namespace godot;

    BIND_GETTER_SETTER_DEFAULT(BTGraphNodeSubtree, file_path);
}