#ifndef BT_DYNAMIC_SEQUENCE_HPP
#define BT_DYNAMIC_SEQUENCE_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

/**
 * @class BTDynamicSequence
 * @brief Behaviour Tree composite that re-evaluates all children every tick.
 *
 * The Dynamic Sequence executes each child from first to last every frame.
 * It succeeds only if all children succeed, but unlike a normal Sequence,
 * it restarts evaluation from the first child on each tick.
 * This ensures that changes in earlier conditions are immediately respected.
 *
 * Return Conditions:
 *
 * - SUCCESS: All children return SUCCESS.
 *
 * - RUNNING: A child returns RUNNING.
 *
 * - FAILURE: A child returns FAILURE.
 */
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

#endif // BT_DYNAMIC_SEQUENCE_HPP