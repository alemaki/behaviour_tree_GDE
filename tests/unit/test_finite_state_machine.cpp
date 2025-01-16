#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "tests/test_utils/test_macros.hpp"

#include "finite_state_machine/fsm.hpp"

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
        State* running = fsm->create_state();

        fsm->initialize();
        CHECK_EQ(fsm->get_state(), idle);

        CHECK(fsm->transition_to_state(running));
        CHECK_EQ(fsm->get_state(), running);

        CHECK(fsm->transition_to_state(jumping));
        CHECK_EQ(fsm->get_state(), jumping);
    }

    TEST_CASE_FIXTURE(FSMFixture, "State emits enter and exit signals")
    {
        State* running = fsm->create_state();
        SignalWatcher::watch_signals(running);

        fsm->initialize();

        CHECK(fsm->transition_to_state(running));
        CHECK_EQ(fsm->get_state(), running);
        CHECK(SignalWatcher::signal_emitted(running, "entered"));
        CHECK_EQ(SignalWatcher::get_signal_emitted_count(running, "entered"), 1);

        CHECK(fsm->transition_to_state(idle));
        CHECK(SignalWatcher::signal_emitted(running, "exited"));
        CHECK_EQ(SignalWatcher::get_signal_emitted_count(running, "exited"), 1);
    }
}
