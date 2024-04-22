#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_selector.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_fail.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_succeed.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"

TEST_SUITE("Test task execution")
{
    TEST_CASE("Task creation and status setting")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        CHECK(task->get_status() == BTTask::Status::FRESH);

        task->set_status(BTTask::Status::RUNNING);
        CHECK(task->get_status() == BTTask::Status::RUNNING);

        task->set_status(BTTask::Status::SUCCESS);
        CHECK(task->get_status() == BTTask::Status::SUCCESS);
    }

    TEST_CASE("Execute and abort task")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        CHECK(task->get_status() == BTTask::Status::FRESH);

        task->execute(0.1);
        CHECK(task->get_status() == BTTask::Status::FAILURE);

        task->set_status(BTTask::Status::RUNNING);
        task->abort();
        CHECK(task->get_status() == BTTask::Status::FRESH);
    }

    TEST_CASE("Always succeed basic")
    {
        godot::Ref<BTAlwaysSucceed> task = memnew(BTAlwaysSucceed);
        CHECK(task->get_status() == BTTask::Status::FRESH);

        task->execute(0.1);
        CHECK(task->get_status() == BTTask::Status::SUCCESS);
    }

    TEST_CASE("Always fail basic")
    {
        godot::Ref<BTAlwaysFail> task = memnew(BTAlwaysFail);
        CHECK(task->get_status() == BTTask::Status::FRESH);

        task->execute(0.1);
        CHECK(task->get_status() == BTTask::Status::FAILURE);
    }

    TEST_CASE("Always succeed with children")
    {
        godot::Ref<BTAlwaysSucceed> task = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);
        task->add_child(task_fail);

        task->execute(0.1);
        CHECK(task->get_status() == BTTask::Status::SUCCESS);
    }

    TEST_CASE("Always fail with children")
    {
        godot::Ref<BTAlwaysFail> task = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysSucceed> task_succeed = memnew(BTAlwaysSucceed);
        task->add_child(task_succeed);

        task->execute(0.1);
        CHECK(task->get_status() == BTTask::Status::FAILURE);
    }

    TEST_CASE("Sequence task behavior")
    {
        godot::Ref<BTSequence> sequence = memnew(BTSequence);
        godot::Ref<BTAlwaysSucceed> task_succeed1 = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysSucceed> task_succeed2 = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);

        SUBCASE("Sequence succeeds on all children success")
        {
            sequence->add_child(task_succeed1);
            sequence->add_child(task_succeed2);

            BTTask::Status status = sequence->execute(0.1);
            CHECK(status == BTTask::Status::SUCCESS);

            CHECK(task_succeed1->get_status() == BTTask::Status::SUCCESS);
            CHECK(task_succeed2->get_status() == BTTask::Status::SUCCESS);
        }

        SUBCASE("Sequence fails on last child's failure")
        {
            sequence->add_child(task_succeed1);
            sequence->add_child(task_fail);

            BTTask::Status status = sequence->execute(0.1);
            CHECK(status == BTTask::Status::FAILURE);

            CHECK(task_succeed1->get_status() == BTTask::Status::SUCCESS);
            CHECK(task_fail->get_status() == BTTask::Status::FAILURE);

        }

        SUBCASE("Sequence fails early on first child's failure")
        {
            sequence->add_child(task_fail);
            sequence->add_child(task_succeed1);

            BTTask::Status status = sequence->execute(0.1);
            CHECK(status == BTTask::Status::FAILURE);

            CHECK(task_fail->get_status() == BTTask::Status::FAILURE);
            CHECK(task_succeed1->get_status() == BTTask::Status::FRESH);
        }
    }

    TEST_CASE("Selector task behavior") 
    {
        godot::Ref<BTSelector> selector = memnew(BTSelector);
        godot::Ref<BTAlwaysFail> task_fail1 = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysFail> task_fail2 = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysSucceed> task_succeed = memnew(BTAlwaysSucceed);

        SUBCASE("Selector fails on all children failure")
        {
            selector->add_child(task_fail1);
            selector->add_child(task_fail2);

            BTTask::Status status = selector->execute(0.1);
            CHECK(status == BTTask::Status::FAILURE);

            CHECK(task_fail1->get_status() == BTTask::Status::FAILURE);
            CHECK(task_fail2->get_status() == BTTask::Status::FAILURE);
        }

        SUBCASE("Selector succeeds on last child's success")
        {
            selector->add_child(task_fail1);
            selector->add_child(task_succeed);

            BTTask::Status status = selector->execute(0.1);
            CHECK(status == BTTask::Status::SUCCESS);

            CHECK(task_fail1->get_status() == BTTask::Status::FAILURE);
            CHECK(task_succeed->get_status() == BTTask::Status::SUCCESS);
        }

        SUBCASE("Selector succeeds early on first child's success")
        {
            selector->add_child(task_succeed);
            selector->add_child(task_fail1);

            BTTask::Status status = selector->execute(0.1);
            CHECK(status == BTTask::Status::SUCCESS);

            CHECK(task_succeed->get_status() == BTTask::Status::SUCCESS);
            CHECK(task_fail1->get_status() == BTTask::Status::FRESH);
        }
    }

    TEST_CASE("Invert task behavior")
    {
        godot::Ref<BTInvert> invert = memnew(BTInvert);

        SUBCASE("Invert fails")
        {
            godot::Ref<BTAlwaysSucceed> task_succeed = memnew(BTAlwaysSucceed);
            invert->add_child(task_succeed);

            invert->execute(0.1);
            CHECK(invert->get_status() == BTTask::Status::FAILURE);
            CHECK(task_succeed->get_status() == BTTask::Status::SUCCESS);
        }

        SUBCASE("Invert succeeds")
        {
            godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);
            invert->add_child(task_fail);

            invert->execute(0.1);
            CHECK(invert->get_status() == BTTask::Status::SUCCESS);
            CHECK(task_fail->get_status() == BTTask::Status::FAILURE);
        }
    }

    TEST_CASE("Task abort")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        task->set_status(BTTask::Status::RUNNING);
        task->abort();
        CHECK(task->get_status() == BTTask::Status::FRESH);
    }
}