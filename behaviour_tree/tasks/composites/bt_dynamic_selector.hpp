#ifndef BT_DYNAMIC_SELECTOR_HPP
#define BT_DYNAMIC_SELECTOR_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

/**
 * @class BTDynamicSelector
 * @brief Behaviour Tree composite that re-evaluates all children every tick.
 *
 * The Dynamic Selector checks its children from first to last each frame,
 * returning the first child that succeeds or is running.
 * This allows higher-priority tasks to interrupt lower ones when conditions change.
 *
 * Return Conditions:
 *
 * - SUCCESS: A child returns SUCCESS.
 *
 * - RUNNING: A child returns RUNNING.
 *
 * - FAILURE: All children return FAILURE.
 */
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