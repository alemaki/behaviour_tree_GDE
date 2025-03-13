#ifndef BT_SELECTOR_HPP
#define BT_SELECTOR_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

/**
 * @class BTSelector
 * @brief Behaviour Tree composite that executes its children in order until one succeeds.
 *
 * The Selector node tries each child in sequence until one succeeds or is running.
 * It is often used to represent fallback or decision logic.
 * If all children fail, the selector fails.
 * If a child is running, the selector remains running and resumes from that child next tick.
 *
 * Return Conditions:
 *
 * - SUCCESS: A child returns SUCCESS.
 *
 * - RUNNING: A child returns RUNNING.
 *
 * - FAILURE: All children return FAILURE.
 */
class BTSelector : public BTComposite
{
	GDCLASS(BTSelector, BTComposite);

private:
	int last_running_index = 0;

protected:
	virtual void _enter() override;
	virtual Status _tick(double delta) override;

protected:
	static void _bind_methods(){};
};

#endif // BT_SELECTOR_HPP