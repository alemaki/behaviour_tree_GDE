#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>
#include <godot_cpp/classes/node.hpp>

#include "tests/test_utils/test_runner.hpp"
#include "behaviour/conditions/bt_check_value.hpp"
#include "blackboard/blackboard.hpp"

struct CheckValueFixture
{
    godot::Ref<BTCheckValue> task;
    godot::Node* actor;
    godot::Ref<Blackboard> blackboard;

    CheckValueFixture() : task(memnew(BTCheckValue)),
                          actor(memnew(godot::Node)),
                          blackboard(memnew(Blackboard))
    {
        task->set_complain_enabled(false);
        task->set_var_name("TestVar");
        task->initialize(actor, blackboard);
    }

    ~CheckValueFixture()
    {
        memdelete(actor);
    }
};

TEST_SUITE("BTCheckValue")
{
    TEST_CASE_FIXTURE(CheckValueFixture, "Check true value matches")
    {
        task->set_check_true_enabled(true);
        blackboard->set_var("TestVar", true);
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check true value does not match")
    {
        task->set_check_true_enabled(true);
        blackboard->set_var("TestVar", false);
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check integer value matches")
    {
        task->set_check_int_enabled(true);
        task->set_int_value(42);
        blackboard->set_var("TestVar", 42);
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check integer value does not match")
    {
        task->set_check_int_enabled(true);
        task->set_int_value(42);
        blackboard->set_var("TestVar", 24);
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check float value matches")
    {
        task->set_check_float_enabled(true);
        task->set_float_value(3.14f);
        blackboard->set_var("TestVar", 3.14f);
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check float value does not match")
    {
        task->set_check_float_enabled(true);
        task->set_float_value(3.14f);
        blackboard->set_var("TestVar", 2.71f);
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check string value matches")
    {
        task->set_check_string_enabled(true);
        task->set_string_value("Hello");
        blackboard->set_var("TestVar", "Hello");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check string value does not match")
    {
        task->set_check_string_enabled(true);
        task->set_string_value("Hello");
        blackboard->set_var("TestVar", "World");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check string name value matches")
    {
        task->set_check_string_name_enabled(true);
        task->set_string_name_value(godot::StringName("TestName"));
        blackboard->set_var("TestVar", godot::StringName("TestName"));
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check string name value does not match")
    {
        task->set_check_string_name_enabled(true);
        task->set_string_name_value(godot::StringName("TestName"));
        blackboard->set_var("TestVar", godot::StringName("AnotherName"));
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check Vector2 value matches")
    {
        task->set_check_vector2_enabled(true);
        task->set_vector2_value(godot::Vector2(1, 2));
        blackboard->set_var("TestVar", godot::Vector2(1, 2));
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check Vector2 value does not match")
    {
        task->set_check_vector2_enabled(true);
        task->set_vector2_value(godot::Vector2(1, 2));
        blackboard->set_var("TestVar", godot::Vector2(3, 4));
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check Vector3 value matches")
    {
        task->set_check_vector3_enabled(true);
        task->set_vector3_value(godot::Vector3(1, 2, 3));
        blackboard->set_var("TestVar", godot::Vector3(1, 2, 3));
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::SUCCESS);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Check Vector3 value does not match")
    {
        task->set_check_vector3_enabled(true);
        task->set_vector3_value(godot::Vector3(1, 2, 3));
        blackboard->set_var("TestVar", godot::Vector3(4, 5, 6));
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Variable does not exist")
    {
        blackboard->erase_var("TestVar");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Variable type mismatch: expected int, got string")
    {
        task->set_check_int_enabled(true);
        task->set_int_value(42);
        blackboard->set_var("TestVar", "NotAnInt");
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }

    TEST_CASE_FIXTURE(CheckValueFixture, "Variable type mismatch: expected float, got bool")
    {
        task->set_check_float_enabled(true);
        task->set_float_value(3.14f);
        blackboard->set_var("TestVar", true);
        BTTask::Status status = task->execute(0.1);
        CHECK_EQ(status, BTTask::Status::FAILURE);
    }
}