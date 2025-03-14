#ifndef BT_SEQUENCE_HPP
#define BT_SEQUENCE_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

/**
 * @class BTSequence
 * @brief Behaviour Tree composite that executes its children in order until one fails.
 *
 * The Sequence node ensures all its children succeed in sequence before succeeding itself.
 * If any child fails, the entire sequence fails immediately.
 * If a child is still running, the sequence remains running and resumes from that child next tick.
 *
 * Return Conditions:
 *
 * - SUCCESS: All children return SUCCESS.
 *
 * - RUNNING: A child returns RUNNING.
 *
 * - FAILURE: A child returns FAILURE.
 */
class BTSequence : public BTComposite
{
	GDCLASS(BTSequence, BTComposite);

private:
	int last_running_index = 0;

protected:
	virtual void _enter() override;
	virtual Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_SEQUENCE_HPP