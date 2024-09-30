#include "utils.hpp" 


godot::SceneTree* get_scene_tree()
{
    return godot::Object::cast_to<godot::SceneTree>(godot::Engine::get_singleton()->get_main_loop());
}

godot::Node* get_scene_root()
{
    return get_scene_tree()->get_current_scene();
}

namespace utils
{

godot::Vector<godot::StringName> get_subclasses(const godot::StringName& base_class_name)
{
    godot::Vector<godot::StringName> subclasses;

    godot::PackedStringArray class_list;
    class_list = godot::ClassDB::get_class_list();

    for (const godot::StringName &class_name : class_list)
    {
        if (godot::ClassDB::is_parent_class(class_name, base_class_name))
        {
            subclasses.append(class_name);
        }
    }

    return subclasses;
}


bool is_subclass(const godot::StringName& class_name, const godot::StringName& base_class_name)
{
    godot::Vector<godot::StringName> subclasses = get_subclasses(base_class_name);

    if (subclasses.has(class_name))
    {
        return true;
    }

    return false;
}

}