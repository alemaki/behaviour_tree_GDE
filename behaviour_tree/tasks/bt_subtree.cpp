#include "bt_subtree.hpp"

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "behaviour_tree/utils/utils.hpp"

BehaviourTree* BTSubtree::load_behavior_tree() const
{
    return utils::load_scene_node<BehaviourTree>(this->file_path);
}

godot::Ref<BTTask> BTSubtree::clone() const
{
    BehaviourTree* temp_behaviour_tree = this->load_behavior_tree();
    if (temp_behaviour_tree == nullptr)
    {
        return nullptr;
    }

    if (temp_behaviour_tree->get_root_task() == nullptr)
    {
        godot::UtilityFunctions::push_error(vformat("%s: Behaviour tree doesn't have any tasks:", this->get_name(), this->file_path));
        return nullptr;
    }

    godot::Ref<BTTask> subtree = temp_behaviour_tree->get_root_task()->clone();

    memdelete(temp_behaviour_tree);

    return subtree;
}

void BTSubtree::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("load_behavior_tree"), &BTSubtree::load_behavior_tree);

    BIND_GETTER_SETTER_PROPERTY_DEFAULT(BTSubtree, STRING, file_path);
}