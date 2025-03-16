#ifndef BT_RANDOM_SELECTOR_HPP
#define BT_RANDOM_SELECTOR_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"
/**
 * @class BTRandomSelector
 * @brief Behaviour Tree composite that executes its children in a random order.
 *
 * The Random Selector shuffles its children each time it begins execution
 * and evaluates them one by one until one succeeds or is running.
 * This allows non-deterministic behaviour, useful for varied AI decisions
 * such as random patrol paths, attack choices, or idle actions.
 *
 * Return Conditions:
 *
 * - SUCCESS: A child returns SUCCESS.
 *
 * - RUNNING: A child returns RUNNING.
 *
 * - FAILURE: All children return FAILURE.
 */
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