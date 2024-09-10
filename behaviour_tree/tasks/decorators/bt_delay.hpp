#ifndef BT_DELAY_HPP
#define BT_DELAY_HPP

#include <godot_cpp/classes/scene_tree_timer.hpp>

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTDelay : public BTDecorator
{
	GDCLASS(BTDelay, BTDecorator);

private:
	bool delay_active;
	bool execute_child = false;
	godot::Ref<godot::SceneTreeTimer> delay_timer = nullptr;
	double seconds = 1.0;

private:
	void start_delay();

protected:
	virtual void _enter();
	virtual Status _tick(double delta) override;

	void _on_timeout();

public:
	CREATE_GETTER_SETTER_DEFAULT(double, seconds);
	_FORCE_INLINE_ bool is_delay_active() const { return this->delay_active; }

protected:
	static void _bind_methods();
};

#endif // BT_DELAY_HPP