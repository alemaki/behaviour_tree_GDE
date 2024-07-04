#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>
#include <godot_cpp/classes/node.hpp>

#include "tests/test_utils/test_runner.hpp"
#include "conditions/bt_check_fsm_state.hpp"
#include "blackboard/blackboard.hpp"
#include "finite_state_machine/fsm.hpp"

struct CheckFSMStateFixture
{
    godot::Ref<BTCheckFSMState> task;
    godot::Node* actor;
    godot::Ref<Blackboard> blackboard;
    FSM* fsm;

    CheckFSMStateFixture() : task(memnew(BTCheckFSMState)),
                             actor(memnew(godot::Node)),
                             blackboard(memnew(Blackboard)),
                             fsm(memnew(FSM))
    {
        task->set_complain(false);
        task->set_state("running");
        task->set_fsm_name("TestFSM");
        
        fsm->set_initial_state("idle");
        fsm->add_transition("idle", "running");

        blackboard->set_var("TestFSM", fsm);

        task->initialize(actor, blackboard);
    }

    ~CheckFSMStateFixture()
    {
        memdelete(actor);
    };
};

TEST_SUITE("BTCheckFSMState")
{
    TEST_CASE_FIXTURE(CheckFSMStateFixture, "FSM state matches")
    {
        fsm->transition_to("running");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckFSMStateFixture, "FSM state does not match")
    {
        fsm->transition_to("idle");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckFSMStateFixture, "FSM does not exist in Blackboard")
    {
        blackboard->erase_var("TestFSM");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }
}