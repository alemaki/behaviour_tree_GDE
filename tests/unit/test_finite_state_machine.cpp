#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "finite_state_machine/fsm.hpp"
#include "tests/test_utils/test_macros.hpp"

struct FSMFixture
{
    FSM* fsm;
    State* idle;
    State* running;
    FSMFixture()
    {
        fsm = memnew(FSM);
        idle = fsm->create_state();
        running = fsm->create_state();
        fsm->set_initial_state(idle);
    }
    ~FSMFixture()
    {
        memdelete(fsm);
    }
};

TEST_SUITE("FSM")
{
    TEST_CASE_FIXTURE(FSMFixture, "Set and get initial state")
    {
        fsm->initialize();

        CHECK_EQ(fsm->get_state(), idle);
    }

    TEST_CASE_FIXTURE(FSMFixture, "Add state and transition to valid state")
    {
        fsm->initialize();

        CHECK(fsm->transition_to_state(running));
        CHECK_EQ(fsm->get_state(), running);
    }

    TEST_CASE_FIXTURE(FSMFixture, "Fail to transition to invalid state")
    {
        State* jumping = memnew(State);

        fsm->initialize();

        CHECK_FALSE(fsm->transition_to_state(jumping));
        CHECK_EQ(fsm->get_state(), idle);

        memdelete(jumping);
    }

    TEST_CASE_FIXTURE(FSMFixture, "Initialize FSM and verify state set")
    {
        State* jumping = fsm->create_state();

        fsm->initialize();
        CHECK_EQ(fsm->get_state(), idle);

        CHECK(fsm->transition_to_state(running));
        CHECK_EQ(fsm->get_state(), running);

        CHECK(fsm->transition_to_state(jumping));
        CHECK_EQ(fsm->get_state(), jumping);
    }

    TEST_CASE_FIXTURE(FSMFixture, "State emits enter and exit signals")
    {
        SignalObserver::watch_signals(running);
        SignalObserver::watch_signals(idle);

        fsm->initialize();

        CHECK(fsm->transition_to_state(running));
        CHECK_EQ(fsm->get_state(), running);

        CHECK_FALSE(SignalObserver::signal_emitted(running, "exited"));
        CHECK_EQ(SignalObserver::get_signal_emitted_count(idle, "entered"), 1);
        CHECK_EQ(SignalObserver::get_signal_emitted_count(running, "entered"), 1);
        CHECK_EQ(SignalObserver::get_signal_emitted_count(idle, "exited"), 1);

        CHECK(fsm->transition_to_state(idle));
        CHECK_EQ(SignalObserver::get_signal_emitted_count(idle, "entered"), 2);
        CHECK_EQ(SignalObserver::get_signal_emitted_count(running, "entered"), 1);
        CHECK_EQ(SignalObserver::get_signal_emitted_count(idle, "exited"), 1);
        CHECK_EQ(SignalObserver::get_signal_emitted_count(running, "exited"), 1);
    }

    static int process_call_count = 0;
    static int process_total_delta = 0;
    void on_static_process(double delta)
    {
        process_call_count++;
        process_total_delta += delta;
    }

    TEST_CASE_FIXTURE(FSMFixture, "State emits the process callables when process is called")
    {
        process_call_count = 0;
        process_total_delta = 0;
        godot::Callable callable = callable_mp_static(&on_static_process);
        idle->add_process_callable(callable);
        fsm->initialize();
        REQUIRE(fsm->get_state() == idle);
        fsm->process_state(10);
        CHECK_EQ(process_call_count, 1);
        CHECK_EQ(process_total_delta, doctest::Approx(10));
    }
}
