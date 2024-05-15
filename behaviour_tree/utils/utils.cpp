#include "utils.hpp" 

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

}