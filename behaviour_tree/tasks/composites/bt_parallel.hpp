#ifndef BT_PARALLEL_HPP
#define BT_PARALLEL_HPP

#include "behaviour_tree/tasks/bt_composite.hpp"

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