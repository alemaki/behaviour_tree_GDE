#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include <godot_cpp/classes/scene_tree.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/composites/bt_dynamic_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_dynamic_selector.hpp"
#include "behaviour_tree/tasks/composites/bt_parallel.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"
#include "behaviour_tree/tasks/composites/bt_selector.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_fail.hpp"
#include "behaviour_tree/tasks/decorators/bt_always_succeed.hpp"
#include "behaviour_tree/tasks/decorators/bt_cooldown.hpp"
#include "behaviour_tree/tasks/decorators/bt_delay.hpp"
#include "behaviour_tree/tasks/decorators/bt_cooldown.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"

TEST_SUITE("Test task execution")
{
    TEST_CASE("Task creation and status setting")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        CHECK_EQ(task->get_status(), BTTask::Status::FRESH);

        task->set_status(BTTask::Status::RUNNING);
        CHECK_EQ(task->get_status(), BTTask::Status::RUNNING);

        task->set_status(BTTask::Status::SUCCESS);
        CHECK_EQ(task->get_status(), BTTask::Status::SUCCESS);
    }

    TEST_CASE("Execute and abort task")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        CHECK_EQ(task->get_status(), BTTask::Status::FRESH);

        task->execute(0.1);
        CHECK_EQ(task->get_status(), BTTask::Status::FAILURE);

        task->set_status(BTTask::Status::RUNNING);
        task->abort();
        CHECK_EQ(task->get_status(), BTTask::Status::FRESH);
    }

    TEST_CASE("Always succeed basic")
    {
        godot::Ref<BTAlwaysSucceed> task = memnew(BTAlwaysSucceed);
        CHECK_EQ(task->get_status(), BTTask::Status::FRESH);

        task->execute(0.1);
        CHECK_EQ(task->get_status(), BTTask::Status::SUCCESS);
    }

    TEST_CASE("Always fail basic")
    {
        godot::Ref<BTAlwaysFail> task = memnew(BTAlwaysFail);
        CHECK_EQ(task->get_status(), BTTask::Status::FRESH);

        task->execute(0.1);
        CHECK_EQ(task->get_status(), BTTask::Status::FAILURE);
    }

    TEST_CASE("Always succeed with children")
    {
        godot::Ref<BTAlwaysSucceed> task = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);
        task->add_child(task_fail);

        task->execute(0.1);
        CHECK_EQ(task->get_status(), BTTask::Status::SUCCESS);
    }

    TEST_CASE("Always fail with children")
    {
        godot::Ref<BTAlwaysFail> task = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysSucceed> task_succeed = memnew(BTAlwaysSucceed);
        task->add_child(task_succeed);

        task->execute(0.1);
        CHECK_EQ(task->get_status(), BTTask::Status::FAILURE);
    }

    TEST_CASE("Delay task behaviour")
    {
        godot::Ref<BTDelay> delay_task = memnew(BTDelay);
        delay_task->set_seconds(1.0);

        godot::Ref<BTAlwaysSucceed> task_succeed = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);

        SUBCASE("Starts delay and executes child on timeout")
        {
            delay_task->add_child(task_succeed);
            CHECK_EQ(delay_task->get_status(), BTTask::Status::FRESH);

            BTTask::Status status = delay_task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::RUNNING);
            CHECK_EQ(delay_task->is_delay_active(), true);

            delay_task->call("_on_timeout");
            status = delay_task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);
            CHECK_EQ(delay_task->is_delay_active(), false);
        }

        SUBCASE("Delay task remains running during delay")
        {
            delay_task->add_child(task_succeed);

            BTTask::Status status = delay_task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::RUNNING);
            CHECK_EQ(delay_task->is_delay_active(), true);

            status = delay_task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::RUNNING);
            CHECK_EQ(delay_task->is_delay_active(), true);
        }

        SUBCASE("Child task fails after delay")
        {
            delay_task->add_child(task_fail);

            BTTask::Status status = delay_task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::RUNNING);
            CHECK_EQ(delay_task->is_delay_active(), true);

            delay_task->call("_on_timeout");

            status = delay_task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);
            CHECK_EQ(delay_task->is_delay_active(), false);
        }

        SUBCASE("Delay task resets on timeout")
        {
            delay_task->add_child(task_succeed);

            delay_task->execute(0.1);
            CHECK_EQ(delay_task->is_delay_active(), true);

            delay_task->call("_on_timeout");

            CHECK_EQ(delay_task->is_delay_active(), false);
            BTTask::Status status = delay_task->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);
        }

        SUBCASE("Task does not proceed if delay is active")
        {
            delay_task->add_child(task_succeed);

            delay_task->execute(0.1);
            CHECK_EQ(delay_task->is_delay_active(), true);

            CHECK_EQ(task_succeed->get_status(), BTTask::Status::FRESH);
        }
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
            CHECK_EQ(status, BTTask::Status::SUCCESS);

            CHECK_EQ(task_succeed1->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_succeed2->get_status(), BTTask::Status::SUCCESS);
        }

        SUBCASE("Sequence fails on last child's failure")
        {
            sequence->add_child(task_succeed1);
            sequence->add_child(task_fail);

            BTTask::Status status = sequence->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);

            CHECK_EQ(task_succeed1->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_fail->get_status(), BTTask::Status::FAILURE);

        }

        SUBCASE("Sequence fails early on first child's failure")
        {
            sequence->add_child(task_fail);
            sequence->add_child(task_succeed1);

            BTTask::Status status = sequence->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);

            CHECK_EQ(task_fail->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_succeed1->get_status(), BTTask::Status::FRESH);
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
            CHECK_EQ(status, BTTask::Status::FAILURE);

            CHECK_EQ(task_fail1->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_fail2->get_status(), BTTask::Status::FAILURE);
        }

        SUBCASE("Selector succeeds on last child's success")
        {
            selector->add_child(task_fail1);
            selector->add_child(task_succeed);

            BTTask::Status status = selector->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);

            CHECK_EQ(task_fail1->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_succeed->get_status(), BTTask::Status::SUCCESS);
        }

        SUBCASE("Selector succeeds early on first child's success")
        {
            selector->add_child(task_succeed);
            selector->add_child(task_fail1);

            BTTask::Status status = selector->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);

            CHECK_EQ(task_succeed->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_fail1->get_status(), BTTask::Status::FRESH);
        }
    }

    TEST_CASE("Dynamic Selector task behavior") 
    {
        godot::Ref<BTDynamicSelector> dynamic_selector = memnew(BTDynamicSelector);
        godot::Ref<BTAlwaysFail> task_fail1 = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysFail> task_fail2 = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysSucceed> task_succeed = memnew(BTAlwaysSucceed);
        godot::Ref<BTDelay> task_running = memnew(BTDelay);

        SUBCASE("Dynamic selector succeeds when last child succeeds")
        {
            dynamic_selector->add_child(task_fail1);
            dynamic_selector->add_child(task_succeed);

            BTTask::Status status = dynamic_selector->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);

            CHECK_EQ(task_fail1->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_succeed->get_status(), BTTask::Status::SUCCESS);
        }

        SUBCASE("Dynamic selector runs until success is found")
        {
            dynamic_selector->add_child(task_fail1);
            dynamic_selector->add_child(task_running);
            dynamic_selector->add_child(task_succeed);

            BTTask::Status status = dynamic_selector->execute(0.1);
            CHECK_EQ(status, BTTask::Status::RUNNING);

            CHECK_EQ(task_fail1->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_running->get_status(), BTTask::Status::RUNNING);
            CHECK_EQ(task_succeed->get_status(), BTTask::Status::FRESH);
        }

        SUBCASE("Dynamic selector aborts previously running task")
        {
            dynamic_selector->add_child(task_fail1);
            dynamic_selector->add_child(task_running);

            BTTask::Status status = dynamic_selector->execute(0.1);
            CHECK_EQ(status, BTTask::Status::RUNNING);
            CHECK_EQ(task_fail1->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_running->get_status(), BTTask::Status::RUNNING);

            dynamic_selector->remove_child_at_index(0);
            dynamic_selector->add_child_at_index(task_succeed, 0);

            status = dynamic_selector->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);
            CHECK_EQ(task_succeed->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_running->get_status(), BTTask::Status::FRESH);
        }

        SUBCASE("Dynamic selector fails on all child failures")
        {
            dynamic_selector->add_child(task_fail1);
            dynamic_selector->add_child(task_fail2);

            BTTask::Status status = dynamic_selector->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);

            CHECK_EQ(task_fail1->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_fail2->get_status(), BTTask::Status::FAILURE);
        }
    }

    TEST_CASE("Dynamic Sequence task behavior") 
    {
        godot::Ref<BTDynamicSequence> dynamic_sequence = memnew(BTDynamicSequence);
        godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysSucceed> task_succeed1 = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysSucceed> task_succeed2 = memnew(BTAlwaysSucceed);
        godot::Ref<BTDelay> task_running = memnew(BTDelay);

        SUBCASE("Dynamic sequence fails when any child fails")
        {
            dynamic_sequence->add_child(task_succeed1);
            dynamic_sequence->add_child(task_fail);

            BTTask::Status status = dynamic_sequence->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);

            CHECK_EQ(task_succeed1->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_fail->get_status(), BTTask::Status::FAILURE);
        }

        SUBCASE("Dynamic sequence succeeds when all children succeed")
        {
            dynamic_sequence->add_child(task_succeed1);
            dynamic_sequence->add_child(task_succeed2);

            BTTask::Status status = dynamic_sequence->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);

            CHECK_EQ(task_succeed1->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_succeed2->get_status(), BTTask::Status::SUCCESS);
        }

        SUBCASE("Dynamic sequence runs until task is running")
        {
            dynamic_sequence->add_child(task_succeed1);
            dynamic_sequence->add_child(task_running);

            BTTask::Status status = dynamic_sequence->execute(0.1);
            CHECK_EQ(status, BTTask::Status::RUNNING);
            CHECK_EQ(task_succeed1->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_running->get_status(), BTTask::Status::RUNNING);
            dynamic_sequence->remove_child_at_index(0);
            dynamic_sequence->add_child_at_index(task_fail, 0);

            status = dynamic_sequence->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);
            CHECK_EQ(task_fail->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_running->get_status(), BTTask::Status::FRESH);
        }

        SUBCASE("Dynamic sequence aborts previously running task")
        {
            dynamic_sequence->add_child(task_succeed1);
            dynamic_sequence->add_child(task_running);

            dynamic_sequence->execute(0.1);
            CHECK_EQ(task_running->get_status(), BTTask::Status::RUNNING);

            BTTask::Status status = dynamic_sequence->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);
            CHECK_EQ(task_running->get_status(), BTTask::Status::FRESH);
            CHECK_EQ(task_succeed2->get_status(), BTTask::Status::SUCCESS);
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
            CHECK_EQ(invert->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(task_succeed->get_status(), BTTask::Status::SUCCESS);
        }

        SUBCASE("Invert succeeds")
        {
            godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);
            invert->add_child(task_fail);

            invert->execute(0.1);
            CHECK_EQ(invert->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(task_fail->get_status(), BTTask::Status::FAILURE);
        }
    }

    TEST_CASE("Cooldown task behaviour")
    {
        godot::Ref<BTCooldown> cooldown_task = memnew(BTCooldown);
        cooldown_task->set_duration(1.0);

        godot::Ref<BTAlwaysSucceed> task_succeed = memnew(BTAlwaysSucceed);

        godot::Ref<BTAlwaysFail> task_fail = memnew(BTAlwaysFail);

        SUBCASE("Enters cooldown after success")
        {
            cooldown_task->add_child(task_succeed);
            CHECK_EQ(cooldown_task->get_status(), BTTask::Status::FRESH);

            cooldown_task->execute(0.1);
            CHECK_EQ(cooldown_task->get_status(), BTTask::Status::SUCCESS);
            CHECK_EQ(cooldown_task->is_cooldown_active(), true);
        }

        SUBCASE("Cooldown task fails and cools down when trigger_on_failure is true")
        {
            cooldown_task->set_trigger_on_failure(true);
            cooldown_task->add_child(task_fail);

            CHECK_EQ(cooldown_task->get_status(), BTTask::Status::FRESH);

            cooldown_task->execute(0.1);
            CHECK_EQ(cooldown_task->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(cooldown_task->is_cooldown_active(), true);
        }

        SUBCASE("Cooldown task does not cool down on failure when trigger_on_failure is false")
        {
            cooldown_task->set_trigger_on_failure(false);
            cooldown_task->add_child(task_fail);

            CHECK_EQ(cooldown_task->get_status(), BTTask::Status::FRESH);

            cooldown_task->execute(0.1);
            CHECK_EQ(cooldown_task->get_status(), BTTask::Status::FAILURE);
            CHECK_EQ(cooldown_task->is_cooldown_active(), false);
        }

        SUBCASE("Cooldown task starts cooled")
        {
            godot::Node* actor = memnew(godot::Node);
            godot::Ref<Blackboard> blackboard = memnew(Blackboard);
            cooldown_task->set_duration(1.0);
            cooldown_task->set_start_cooled(true);
            cooldown_task->initialize(actor, blackboard);

            CHECK_EQ(cooldown_task->is_cooldown_active(), true);

            memdelete(actor);
        }

        SUBCASE("Cooldown resets on timeout")
        {
            cooldown_task->add_child(task_succeed);

            cooldown_task->execute(0.1);
            CHECK_EQ(cooldown_task->is_cooldown_active(), true);

            cooldown_task->call("_on_timeout");

            CHECK_EQ(cooldown_task->is_cooldown_active(), false);
        }
        
        SUBCASE("Task won't execute in a cooldown state")
        {
            cooldown_task->add_child(task_succeed);
            cooldown_task->execute(0.1);
            task_succeed->set_status(BTTask::Status::FRESH);

            CHECK_EQ(cooldown_task->execute(0.1), BTTask::Status::FAILURE);
            CHECK_EQ(cooldown_task->is_cooldown_active(), true);
            CHECK_EQ(task_succeed->get_status(), BTTask::Status::FRESH);
        }
    }

    TEST_CASE("BTParallel succeeds when enough children succeed")
    {
        godot::Ref<BTParallel> parallel = memnew(BTParallel);
        godot::Ref<BTAlwaysSucceed> task_succeed1 = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysSucceed> task_succeed2 = memnew(BTAlwaysSucceed);
        godot::Ref<BTAlwaysFail> task_fail1 = memnew(BTAlwaysFail);
        godot::Ref<BTAlwaysFail> task_fail2 = memnew(BTAlwaysFail);

        parallel->set_successes_required(2);
        parallel->set_failures_required(2);
        parallel->set_repeat(false);
        
        SUBCASE("BTParallel succeeds when enough children succeed")
        {
            parallel->add_child(task_succeed1);
            parallel->add_child(task_fail1);
            parallel->add_child(task_succeed2);

            BTTask::Status status = parallel->execute(0.1);
            CHECK_EQ(status, BTTask::Status::SUCCESS);
        }

        SUBCASE("BTParallel fails when enough children fail")
        {
            parallel->add_child(task_fail1);
            parallel->add_child(task_fail2);
            parallel->add_child(task_succeed1);

            BTTask::Status status = parallel->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);
        }

        SUBCASE("BTParallel fails when not enough succeed or fail")
        {
            parallel->add_child(task_fail1);
            parallel->add_child(task_succeed1);

            BTTask::Status status = parallel->execute(0.1);
            CHECK_EQ(status, BTTask::Status::FAILURE);
        }
    }

    TEST_CASE("Task abort")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        task->set_status(BTTask::Status::RUNNING);
        task->abort();
        CHECK_EQ(task->get_status(), BTTask::Status::FRESH);
    }
}