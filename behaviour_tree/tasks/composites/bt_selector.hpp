#ifndef BT_SELECTOR_H
#define BT_SELECTOR_H

#include "behaviour_tree/tasks/bt_composite.hpp"

class BTSelector : public BTComposite
{
	GDCLASS(BTSelector, BTComposite);

private:
	int last_running_index = 0;

protected:
	virtual void _enter() override;
	virtual Status _tick(double p_delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_SELECTOR_H