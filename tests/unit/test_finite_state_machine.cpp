#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "finite_state_machine/fsm.hpp"

struct FSMFixture
{
    FSM* fsm;
    FSMFixture() : fsm(memnew(FSM))
    {
        fsm->add_state("idle");
        fsm->set_initial_state("idle");
    }
    ~FSMFixture()
    {
        memdelete(this->fsm);
    }
};

TEST_SUITE("FSM")
{
    TEST_CASE_FIXTURE(FSMFixture, "Set and get initial state")
    {
        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Add state and transition to valid state")
    {
        fsm->add_state("running");

        CHECK(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "running");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Fail to transition to invalid state")
    {
        fsm->add_state("running");

        CHECK_FALSE(fsm->transition_to("jumping"));
        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Set and get states")
    {
        godot::Array states;
        states.push_back("jumping");
        states.push_back("running");
        states.push_back("idle");

        fsm->set_states(states);

        godot::Array stored_states = fsm->get_states();
        CHECK_EQ(stored_states.size(), 3);
        CHECK(stored_states.has("jumping"));
        CHECK(stored_states.has("running"));
        CHECK(stored_states.has("idle"));
    }

    TEST_CASE_FIXTURE(FSMFixture, "Transition after setting transitions via array")
    {
        godot::Array states;
        states.push_back("jumping");
        states.push_back("running");
        states.push_back("idle");

        fsm->set_states(states);

        CHECK(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "running");

        CHECK(fsm->transition_to("jumping"));
        CHECK_EQ(fsm->get_state(), "jumping");

        CHECK_FALSE(fsm->transition_to("nothing"));
        CHECK_EQ(fsm->get_state(), "jumping");
    }
}