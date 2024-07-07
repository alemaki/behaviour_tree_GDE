#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "finite_state_machine/fsm.hpp"

struct FSMFixture
{
    FSM* fsm;
    FSMFixture() : fsm(memnew(FSM))
    {
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

    TEST_CASE_FIXTURE(FSMFixture, "Add transition and transition to valid state")
    {
        fsm->add_transition("idle", "running");

        CHECK(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "running");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Add transition and fail to transition to invalid state")
    {
        fsm->add_transition("idle", "running");

        CHECK_FALSE(fsm->transition_to("jumping"));
        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE_FIXTURE(FSMFixture, "Set initial state only once")
    {
        fsm->set_initial_state("running");

        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE("Transition without initial state")
    {
        FSM* fsm = memnew(FSM);

        fsm->add_transition("idle", "running");

        CHECK_FALSE(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "");

        memdelete(fsm);
    }

    TEST_CASE_FIXTURE(FSMFixture, "Set and get transitions")
    {
        godot::Dictionary transitions;
        godot::Array running_transitions;
        running_transitions.push_back("jumping");
        transitions["running"] = running_transitions;

        godot::Array idle_transitions;
        idle_transitions.push_back("running");
        transitions["idle"] = idle_transitions;

        fsm->set_transitions(transitions);

        godot::Dictionary stored_transitions = fsm->get_transitions();
        CHECK_EQ(stored_transitions.size(), 2);
        CHECK_EQ(stored_transitions["idle"], idle_transitions);
        CHECK_EQ(stored_transitions["running"], running_transitions);
    }

    TEST_CASE_FIXTURE(FSMFixture, "Transition after setting transitions via dictionary")
    {
        godot::Dictionary transitions;
        godot::Array idle_to_running;
        idle_to_running.push_back("running");
        transitions["idle"] = idle_to_running;

        fsm->set_initial_state("idle");
        fsm->set_transitions(transitions);

        CHECK(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "running");

        CHECK_FALSE(fsm->transition_to("jumping"));
        CHECK_EQ(fsm->get_state(), "running");
    }
}