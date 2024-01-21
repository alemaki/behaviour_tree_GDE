#ifndef BT_REPEAT_HPP
#define BT_REPEAT_HPP

#include "behaviour_tree/tasks/bt_decorator.hpp"

class BTRepeat : public BTDecorator
{
	GDCLASS(BTRepeat, BTDecorator);
private:
    int repeat_times = 1;
    bool abort_on_failure = false;
    bool abort_on_success = false;
    bool run_forever = false;

    int current_repeat_times = 0;

protected:
    virtual void _enter() override;
    virtual BTTask::Status _tick(double delta) override;

public:
    void set_repeat_times(int repeat_times);
    _FORCE_INLINE_ int get_repeat_times() const
    {
        return this->repeat_times;
    }
    void set_abort_on_failure(bool abort_on_failure);
    _FORCE_INLINE_ bool get_abort_on_failure() const
    {
        return this->abort_on_failure;
    }
    void set_abort_on_success(bool abort_on_success);
    _FORCE_INLINE_ bool get_abort_on_success() const
    {
        return this->abort_on_success;
    }
    void set_run_forever(bool run_forever);
    _FORCE_INLINE_ bool get_run_forever() const
    {
        return this->run_forever;
    }

protected:
    static void _bind_methods();
};

#endif // BT_REPEAT_HPP