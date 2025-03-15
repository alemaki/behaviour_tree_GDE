#ifndef BT_RANDOM_SEQUENCE_H
#define BT_RANDOM_SEQUENCE_H

#include "behaviour_tree/tasks/bt_composite.hpp"

/**
 * @class BTRandomSequence
 * @brief Behaviour Tree composite that executes all children in a random order.
 *
 * The Random Sequence shuffles its children each time execution begins
 * and evaluates them one by one.
 * The sequence only succeeds if all children succeed.
 * If any child fails, the sequence fails immediately.
 * If a child is running, the sequence remains running and resumes from that child next tick.
 *
 * Return Conditions:
 *
 * - SUCCESS: All children return SUCCESS.
 *
 * - RUNNING: A child returns RUNNING.
 *
 * - FAILURE: A child returns FAILURE.
 */
class BTRandomSequence : public BTComposite
{
	GDCLASS(BTRandomSequence, BTComposite);

private:
	int last_running_index = 0;
	godot::Array indicies;

protected:
	virtual void _enter() override;
	virtual Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_RANDOM_SEQUENCE_H