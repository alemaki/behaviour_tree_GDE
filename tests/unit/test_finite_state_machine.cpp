#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "finite_state_machine/fsm.hpp"

struct FSMFixture
{
    FSM* fsm;
    FSMFixture() : fsm(memnew(FSM)){}
    ~FSMFixture()
    {
        memdelete(this->fsm);
    }
};

TEST_SUITE("FSM")
{
    TEST_CASE_FIXTURE(FSMFixture, "Set and get initial state")
    {
        fsm->set_initial_state("idle");
        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Add transition and transition to valid state")
    {
        fsm->set_initial_state("idle");
        fsm->add_transition("idle", "running");

        CHECK(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "running");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Add transition and fail to transition to invalid state")
    {
        fsm->set_initial_state("idle");
        fsm->add_transition("idle", "running");

        CHECK_FALSE(fsm->transition_to("jumping"));
        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Set initial state only once")
    {
        fsm->set_initial_state("idle");
        fsm->set_initial_state("running");

        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Transition without initial state")
    {
        fsm->add_transition("idle", "running");

        CHECK_FALSE(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "");
    }
}