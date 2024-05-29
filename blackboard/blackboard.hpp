#ifndef BLACKBOARD_HPP
#define BLACKBOARD_HPZP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

class Blackboard : public RefCounted {
	GDCLASS(Blackboard, RefCounted);

private:
	HashMap<StringName, Variant> data;

public:
	Variant get_var(const StringName &name, const Variant &default_var = Variant(), bool complain = true) const;
	void set_var(const StringName &name, const Variant &value);
	_FORCE_INLINE_ bool has_var(const StringName &name) const
	{
		return this->data.has(name);
	}
	void erase_var(const StringName &name);
	void clear();
	TypedArray<StringName> list_vars() const;

	Dictionary get_vars_as_dict() const;

protected:
	static void _bind_methods();
};

#endif // BLACKBOARD_H
