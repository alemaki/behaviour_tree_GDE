#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>
#include <godot_cpp/classes/node.hpp>

#include "tests/test_utils/test_runner.hpp"
#include "conditions/bt_check_int.hpp"
#include "blackboard/blackboard.hpp"

TEST_SUITE("BTCheckInt")
{
    TEST_CASE("Basics")
    {
        godot::Ref<BTCheckInt> task = memnew(BTCheckInt);
        task->set_value(10);
        task->set_var_name("TestVar");

        godot::Node* actor = memnew(godot::Node);
        
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        blackboard->set_var("TestVar", 10);

        task->initialize(actor, blackboard);

        SUBCASE("Value matches")
        {
            BTTask::Status status = task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);
        }

        SUBCASE("Value does not match")
        {
            blackboard->set_var("TestVar", 5);
            BTTask::Status status = task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);
        }

        SUBCASE("Variable does not exist")
        {
            blackboard->erase_var("TestVar");
            BTTask::Status status = task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);
        }

        memdelete(actor);
    }
}