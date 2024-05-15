#ifndef BT_UTILS_HPP
#define BT_UTILS_HPP

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
namespace utils
{

template<typename T>
T* load_scene_node(const godot::String& scene_path)
{
    if(!scene_path.ends_with(".tscn") && !scene_path.ends_with(".scn"))
    {
        godot::UtilityFunctions::printerr("File path is not a valid scene file: " + scene_path);
        return nullptr;
    }

    godot::Ref<godot::PackedScene> packed_scene = godot::ResourceLoader::get_singleton()->load(scene_path);
    if (packed_scene.is_null())
    {
        godot::UtilityFunctions::printerr("Failed to load scene: ", scene_path);
        return nullptr;
    }

    if(!(packed_scene->can_instantiate()))
    {
        godot::UtilityFunctions::printerr("Cannot instantiate scene: ", scene_path);
        return nullptr;
    }

    godot::Node* node = packed_scene->instantiate();
    if (node == nullptr)
    {
        godot::UtilityFunctions::printerr("Failed to instantiate scene: ", scene_path);
        return nullptr;
    }

    T* result_node = godot::Object::cast_to<T>(node);
    if (result_node == nullptr)
    {
        godot::UtilityFunctions::printerr("Path to scene is not a", T::get_class_static() ," node: ", scene_path);
        return nullptr;
    }

    return result_node;
}

godot::Vector<godot::StringName> get_subclasses(const godot::StringName& base_class_name);

} /* namespace utils*/

#endif /* BT_UTILS_HPP */