#ifndef BT_RANDOM_SELECTOR_HPP
#define BT_RANDOM_SELECTOR_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

class BTRandomSelector: public BTComposite
{
	GDCLASS(BTRandomSelector, BTComposite);

private:
	int last_running_index = 0;
	godot::Array indicies;

protected:
	virtual void _enter() override;
	virtual Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_RANDOM_SELECTOR_HPP