#ifndef BT_UTILS_HPP
#define BT_UTILS_HPP

#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/core/class_db.hpp>

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

#endif /* BT_UTILS_HPP */