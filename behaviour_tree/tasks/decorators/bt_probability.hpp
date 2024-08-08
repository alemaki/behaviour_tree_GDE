#ifndef BT_PROBABILITY_HPP
#define BT_PROBABILITY_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTProbability : public BTDecorator
{
	GDCLASS(BTProbability, BTDecorator);
private:
    double run_chance;

protected:
    virtual BTTask::Status _tick(double delta) override;

public:
    CREATE_GETTER_SETTER_POSITIVE_DEFAULT(double, run_chance);

protected:
    static void _bind_methods();
};

#endif // BT_PROBABILITY_HPP