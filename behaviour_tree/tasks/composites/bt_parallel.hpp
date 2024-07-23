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
    void set_successes_required(int successes_required);
    _FORCE_INLINE_ int get_successes_required() const
    {
        return this->successes_required;
    }
    void set_failures_required(int failures_required);
    _FORCE_INLINE_ int get_failures_required() const
    {
        return this->failures_required;
    }
    void set_repeat(bool repeat);
    _FORCE_INLINE_ bool get_repeat() const
    {
        return this->repeat;
    }

protected:
	static void _bind_methods();

};

#endif // BT_PARALLEL_HPP