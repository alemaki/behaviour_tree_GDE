#ifndef BT_PARALLEL_HPP
#define BT_PARALLEL_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

/**
 * @class BTParallel
 * @brief Behaviour Tree composite that executes all children simultaneously.
 *
 * The Parallel node runs all its children every tick.
 * It can be configured to succeed or fail based on how many children succeed or fail.
 * Optionally, it may repeat running children even after some have finished.
 *
 * Return Conditions:
 *
 * - SUCCESS:
 *   Returned when the number of successful children reaches or exceeds `successes_required`.
 *
 * - RUNNING:
 *   Returned while neither the success nor failure thresholds have been met.
 *   Continues ticking all children each frame.
 *
 * - FAILURE:
 *   Returned when the number of failed children reaches or exceeds `failures_required`.
 *   Also returned if all children have finished and neither threshold is met (when `repeat` is `false`).
 *
 * Properties:
 *
 * - `successes_required`: Number of successful children required for success.
 *
 * - `failures_required`: Number of failed children required for failure.
 *
 * - `repeat`: If true, finished children are re-ticked each frame; if false, they remain as-is.
 */
class BTParallel: public BTComposite
{
	GDCLASS(BTParallel, BTComposite);

private:
	int successes_required = 1;
	int failures_required = 1;
    bool repeat = false;

protected:
	virtual void _enter() override;
	virtual Status _tick(double delta) override;

public:
    CREATE_GETTER_SETTER_POSITIVE_DEFAULT(int, successes_required);
    CREATE_GETTER_SETTER_POSITIVE_DEFAULT(int, failures_required);
    CREATE_GETTER_SETTER_DEFAULT(bool, repeat);

protected:
	static void _bind_methods();

};

#endif // BT_PARALLEL_HPP