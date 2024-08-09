#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>
#include <godot_cpp/classes/node.hpp>

#include "tests/test_utils/test_runner.hpp"
#include "behaviour/actions/bt_transition_fsm.hpp"
#include "blackboard/blackboard.hpp"
#include "finite_state_machine/fsm.hpp"

struct TransitionFSMFixture
{
    godot::Ref<BTTransitionFSM> task;
    godot::Node* actor;
    godot::Ref<Blackboard> blackboard;
    FSM* fsm;

    TransitionFSMFixture() : task(memnew(BTTransitionFSM)),
                             actor(memnew(godot::Node)),
                             blackboard(memnew(Blackboard)),
                             fsm(memnew(FSM))
    {
        task->set_complain_enabled(false);
        task->set_fsm_name("TestFSM");
        task->set_to_state("running");

        fsm->set_initial_state("idle");
        fsm->add_transition("idle", "running");

        blackboard->set_var("TestFSM", fsm);

        task->initialize(actor, blackboard);
    }

    ~TransitionFSMFixture()
    {
        memdelete(actor);
        memdelete(fsm);
    };
};

TEST_SUITE("BTTransitionFSM")
{
    TEST_CASE_FIXTURE(TransitionFSMFixture, "FSM can transition to target state")
    {
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
        CHECK_EQ(fsm->get_state(), "running");
    }

    TEST_CASE_FIXTURE(TransitionFSMFixture, "FSM cannot transition to target state")
    {
        fsm->add_transition("running", "stopped");
        task->set_to_state("stopped");

        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(TransitionFSMFixture, "FSM does not exist in Blackboard")
    {
        blackboard->erase_var("TestFSM");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }
};