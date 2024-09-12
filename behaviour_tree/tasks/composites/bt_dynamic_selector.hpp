#ifndef BT_DYNAMIC_SELECTOR_HPP
#define BT_DYNAMIC_SELECTOR_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

class BTDynamicSelector : public BTComposite
{
	GDCLASS(BTDynamicSelector, BTComposite);

private:
	int last_running_index = 0;

protected:
	virtual void _enter() override;
	virtual Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_DYNAMIC_SELECTOR_HPP