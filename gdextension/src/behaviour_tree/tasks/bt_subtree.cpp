#include "bt_subtree.hpp"

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void BTSubtree::set_file_path(const godot::String& p_file_path)
{
    this->file_path = p_file_path;
}

BehaviourTree* BTSubtree::load_behavior_tree() const
{
    if(!file_path.ends_with(".tscn") && !file_path.ends_with(".scn"))
    {
        godot::UtilityFunctions::printerr("File path is not a valid scene file: " + this->file_path);
        return nullptr;
    }

    godot::Ref<godot::PackedScene> packed_scene = godot::ResourceLoader::get_singleton()->load(this->file_path);

    if (packed_scene.is_null())
    {
        godot::UtilityFunctions::printerr("Failed to load scene: ", this->file_path);
        return nullptr;
    }

    godot::Node* node = packed_scene->instantiate();
    if (node == nullptr)
    {
        godot::UtilityFunctions::printerr("Failed to instantiate scene: ", this->file_path);
        return nullptr;
    }

    BehaviourTree* behaviour_tree = godot::Object::cast_to<BehaviourTree>(node);
    if (behaviour_tree == nullptr)
    {
        godot::UtilityFunctions::printerr("Path to scene is not a behaviour tree node: ", this->file_path);
        return nullptr;
    }

    return behaviour_tree;

}

godot::Ref<BTTask> BTSubtree::clone() const
{
    BehaviourTree* behaviour_tree = this->load_behavior_tree();
    if (behaviour_tree == nullptr)
    {
        return nullptr;
    }

    if (behaviour_tree->get_root_task() == nullptr)
    {
        godot::UtilityFunctions::printerr("Behaviour tree doesn't have any tasks: ", this->file_path);
        return nullptr;
    }

    godot::Ref<BTTask> subtree = behaviour_tree->get_root_task()->clone();
    
    memfree(behaviour_tree);

    return subtree;
}

void BTSubtree::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("set_file_path", "file_path"), &BTSubtree::set_file_path);
    ClassDB::bind_method(D_METHOD("get_file_path"), &BTSubtree::get_file_path);
    ClassDB::bind_method(D_METHOD("load_behavior_tree"), &BTSubtree::load_behavior_tree);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "file_path"), "set_file_path", "get_file_path");
}