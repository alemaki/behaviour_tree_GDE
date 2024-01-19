#ifndef BT_PROBABILITY_HPP
#define BT_PROBABILITY_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTProbability : public BTDecorator
{
	GDCLASS(BTProbability, BTDecorator);
private:
    float run_chance;

public:
    virtual BTTask::Status _tick(double delta) override;

    void set_run_chance(float chance);
    _FORCE_INLINE_ float get_run_chance() const;
};

#endif // BT_PROBABILITY_HPP