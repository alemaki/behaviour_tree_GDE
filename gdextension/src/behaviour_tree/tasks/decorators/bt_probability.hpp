#ifndef BT_PROBABILITY_HPP
#define BT_PROBABILITY_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTProbability : public BTDecorator
{
	GDCLASS(BTProbability, BTDecorator);
private:
    float run_chance;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    void set_run_chance(float chance);
    _FORCE_INLINE_ float get_run_chance() const;

protected:
    static void _bind_methods();
};

#endif // BT_PROBABILITY_HPP