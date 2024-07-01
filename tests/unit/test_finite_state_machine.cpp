#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "finite_state_machine/fsm.hpp"

TEST_SUITE("FSM")
{
    TEST_CASE("Set and get initial state")
    {
        godot::Ref<FSM> fsm = memnew(FSM);

        fsm->set_initial_state("idle");
        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE("Add transition and transition to valid state")
    {
        godot::Ref<FSM> fsm = memnew(FSM);
        
        fsm->set_initial_state("idle");
        fsm->add_transition("idle", "running");

        CHECK(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "running");
    }

    TEST_CASE("Add transition and fail to transition to invalid state")
    {
        godot::Ref<FSM> fsm = memnew(FSM);
        
        fsm->set_initial_state("idle");
        fsm->add_transition("idle", "running");

        CHECK_FALSE(fsm->transition_to("jumping"));
        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE("Set initial state only once")
    {
        godot::Ref<FSM> fsm = memnew(FSM);

        fsm->set_initial_state("idle");
        fsm->set_initial_state("running");

        CHECK_EQ(fsm->get_state(), "idle");
    }

    TEST_CASE("Transition without initial state")
    {
        godot::Ref<FSM> fsm = memnew(FSM);

        fsm->add_transition("idle", "running");

        CHECK_FALSE(fsm->transition_to("running"));
        CHECK_EQ(fsm->get_state(), "");
    }
}