#ifndef BT_SEQUENCE_HPP
#define BT_SEQUENCE_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

class BTDynamicSequence : public BTComposite
{
	GDCLASS(BTDynamicSequence, BTComposite);

private:
	int last_running_index = 0;

protected:
	virtual void _enter() override;
	virtual Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_SEQUENCE_HPP