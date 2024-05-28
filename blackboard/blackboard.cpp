#include "blackboard.hpp"

#include<godot_cpp/variant/utility_functions.hpp>

Variant Blackboard::get_var(const StringName &name, const Variant &default_var = Variant(), bool complain = true) const
{
    if (this->has_var(name))
    {
        return this->data.get(name);
    }
    else if (complain)
    {
        godot::UtilityFunctions::printerr(vformat("Blackboard variable \"%s\" not found."), name);
    }

    return default_var;

}
void Blackboard::set_var(const StringName &name, const Variant &value)
{
    this->data[name] = value;
}

void Blackboard::erase_var(const StringName &name)
{
    this->data.erase(name);
}

void Blackboard::clear()
{
    this->data.clear();
}

TypedArray<StringName> Blackboard::list_vars() const
{
    godot::TypedArray<StringName> var_names;
    var_names.resize(this->data.size());
    int index = 0;
    for (const KeyValue<StringName, Variant>& element : data)
    {
        var_names[index] = element.key;
        index++;
    }
    return var_names;
}

Dictionary Blackboard::get_vars_as_dict() const
{
    godot::Dictionary vars;
    for (const KeyValue<StringName, Variant>& element : data)
    {
        vars[element.key] = element.value;
    }
    return vars;
}


void Blackboard::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("get_var", "name", "default_var", "complain"), &Blackboard::get_var, DEFVAL(Variant()), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("set_var", "name", "value"), &Blackboard::set_var);
    ClassDB::bind_method(D_METHOD("erase_var", "name"), &Blackboard::erase_var);
    ClassDB::bind_method(D_METHOD("clear"), &Blackboard::clear);
    ClassDB::bind_method(D_METHOD("list_vars"), &Blackboard::list_vars);
    ClassDB::bind_method(D_METHOD("get_vars_as_dict"), &Blackboard::get_vars_as_dict);
}