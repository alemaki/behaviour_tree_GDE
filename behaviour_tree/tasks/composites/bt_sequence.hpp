#ifndef BT_SEQUENCE_HPP
#define BT_SEQUENCE_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

class BTSequence : public BTComposite
{
	GDCLASS(BTSequence, BTComposite);

private:
	int last_running_index = 0;

protected:
	virtual void _enter() override;
	virtual Status _tick(double p_delta) override;
};

#endif // BT_SEQUENCE_HPP