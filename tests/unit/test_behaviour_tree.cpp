#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>
#include "behaviour_tree/behaviour_tree.hpp"

TEST_SUITE("BehaviourTreeTests")
{

    TEST_CASE("Test set and get description")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::String description = "Test Behaviour Tree";
        tree->set_description(description);
        CHECK(tree->get_description() == description);
        memfree(tree);
    }

    TEST_CASE("Test add and get task")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task = memnew(BTTask);
        tree->add_task_by_ref(task);
        int task_id = tree->get_task_id(task);
        CHECK(tree->get_task(task_id) == task);
        memfree(tree);
    }

    TEST_CASE("Test has task")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task = memnew(BTTask);
        CHECK_FALSE(tree->has_task(task));
        tree->add_task_by_ref(task);
        CHECK(tree->has_task(task));
        godot::Ref<BTTask> fake_task = memnew(BTTask);
        CHECK_FALSE(tree->has_task(fake_task));
        memfree(tree);
    }

    TEST_CASE("Test remove task")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task = memnew(BTTask);

        tree->add_task_by_ref(task);
        int task_id = tree->get_task_id(task);
        tree->remove_task(task_id);

        CHECK_FALSE(tree->has_task(task));
        CHECK(tree->get_task(task_id) == nullptr);

        tree->add_task_by_ref(task);
        tree->remove_task_by_ref(task);
        CHECK_FALSE(tree->has_task(task));

        memfree(tree);
    }

    TEST_CASE("Test remove task count")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task = memnew(BTTask);
        godot::Ref<BTTask> task1 = memnew(BTTask);
        godot::Ref<BTTask> task2 = memnew(BTTask);
        tree->add_task_by_ref(task);
        tree->add_task_by_ref(task1);
        tree->add_task_by_ref(task2);
        tree->remove_task_by_ref(task);
        CHECK(tree->get_tasks().size() == 2);
        tree->remove_task_by_ref(task1);
        CHECK(tree->get_tasks().size() == 1);
        tree->remove_task_by_ref(task2);
        CHECK(tree->get_tasks().size() == 0);

        memfree(tree);
    }

    TEST_CASE("Test connect and disconnect tasks")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> parent_task = memnew(BTTask);
        godot::Ref<BTTask> child_task = memnew(BTTask);
        tree->add_task_by_ref(parent_task);
        tree->add_task_by_ref(child_task);

        tree->connect_tasks(parent_task, child_task, 0);
        CHECK(parent_task->has_child(child_task));
        CHECK(child_task->get_parent() == parent_task);

        tree->disconnect_tasks(parent_task, child_task);
        CHECK_FALSE(parent_task->has_child(child_task));
        CHECK(child_task->get_parent() == nullptr);

        memfree(tree);
    }

    TEST_CASE("Test clear tasks")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task1 = memnew(BTTask);
        godot::Ref<BTTask> task2 = memnew(BTTask);
        tree->add_task_by_ref(task1);
        tree->add_task_by_ref(task2);
        tree->clear_tasks();
        CHECK(tree->get_tasks().size() == 0);
        CHECK(tree->get_root_task() == nullptr);

        memfree(tree);
    }

    TEST_CASE("Test set and get root task")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> root_task = memnew(BTTask);
        tree->add_task_by_ref(root_task);
        tree->set_root_task(root_task);
        CHECK(tree->get_root_task() == root_task);

        memfree(tree);
    }

    TEST_CASE("Test detach task")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child_task = memnew(BTTask);
        tree->add_task_by_ref(root_task);
        tree->add_task_by_ref(child_task);
        tree->connect_tasks(root_task, child_task, 0);
        tree->detach_task_by_ref(child_task);
        CHECK_FALSE(root_task->has_child(child_task));
        CHECK(child_task->get_parent() == nullptr);

        memfree(tree);
    }

    TEST_CASE("Test set tasks of parent")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> parent_task = memnew(BTTask);
        godot::Ref<BTTask> child_task1 = memnew(BTTask);
        godot::Ref<BTTask> child_task2 = memnew(BTTask);
        tree->add_task_by_ref(parent_task);
        tree->add_task_by_ref(child_task1);
        tree->add_task_by_ref(child_task2);
        godot::Array children_array;
        children_array.push_back(child_task1);
        children_array.push_back(child_task2);
        tree->set_tasks_of_parent(parent_task, children_array);
        godot::Array children = parent_task->get_children();
        REQUIRE(children.size() == 2);
        CHECK(children[0] == child_task1);
        CHECK(children[1] == child_task2);

        memfree(tree);
    }

    TEST_CASE("Test root task initially null")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        CHECK(tree->get_root_task() == nullptr);

        memfree(tree);
    }

    TEST_CASE("Test added task becomes root")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task = memnew(BTTask);
        godot::Ref<BTTask> task1 = memnew(BTTask);
        tree->add_task_by_ref(task);
        CHECK(tree->get_root_task() == task);
        tree->add_task_by_ref(task1);
        CHECK(tree->get_root_task() == task);

        memfree(tree);
    }

    TEST_CASE("Test set root task")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> root_task1 = memnew(BTTask);
        godot::Ref<BTTask> root_task2 = memnew(BTTask);
        tree->add_task_by_ref(root_task1);
        tree->add_task_by_ref(root_task2);
        tree->set_root_task(root_task2);
        CHECK(tree->get_root_task() == root_task2);

        memfree(tree);
    }

    TEST_CASE("Test remove root task")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task = memnew(BTTask);
        godot::Ref<BTTask> task1 = memnew(BTTask);
        tree->add_task_by_ref(task);
        tree->add_task_by_ref(task1);
        godot::Ref<BTTask> root_task = tree->get_root_task();
        tree->remove_task_by_ref(root_task);
        CHECK(tree->get_root_task() != root_task);
        CHECK(tree->get_root_task() != nullptr);

        memfree(tree);
    }

    TEST_CASE("Test get valid id increment")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        int first_id = tree->get_valid_id();
        godot::Ref<BTTask> task = memnew(BTTask);
        tree->add_task_by_ref(task);
        int second_id = tree->get_valid_id();
        CHECK(second_id == first_id + 1);

        memfree(tree);
    }

    TEST_CASE("Test task hierarchy")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child_task1 = memnew(BTTask);
        godot::Ref<BTTask> child_task2 = memnew(BTTask);
        godot::Ref<BTTask> grandchild_task = memnew(BTTask);

        tree->add_task_by_ref(root_task);
        tree->add_task_by_ref(child_task1);
        tree->add_task_by_ref(child_task2);
        tree->add_task_by_ref(grandchild_task);

        tree->connect_tasks(root_task, child_task1, 0);
        tree->connect_tasks(root_task, child_task2, 1);
        tree->connect_tasks(child_task1, grandchild_task, 0);

        CHECK(root_task->has_child(child_task1));
        CHECK(root_task->has_child(child_task2));
        CHECK(child_task1->has_child(grandchild_task));
        CHECK(grandchild_task->get_root() == root_task);
        CHECK(grandchild_task->get_parent() == child_task1);

        memfree(tree);
    }

    TEST_CASE("Test set custom name task by ref")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> task = memnew(BTTask);
        tree->add_task_by_ref(task);
        godot::String new_name = "New Task Name";
        tree->set_custom_name_task_by_ref(task, new_name);

        CHECK(task->get_custom_name() == new_name);

        memfree(tree);
    }

    TEST_CASE("Test swap root task in")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> old_task = memnew(BTTask);
        godot::Ref<BTTask> new_task = memnew(BTTask);

        tree->add_task_by_ref(old_task);

        godot::Ref<BTTask> child_task = memnew(BTTask);
        tree->add_task_by_ref(child_task);
        tree->connect_tasks(old_task, child_task, 0);

        tree->swap_task_in(old_task, new_task);

        CHECK(tree->has_task(new_task));
        CHECK_FALSE(tree->has_task(old_task));
        CHECK(tree->get_root_task() == new_task);
        CHECK(new_task->get_children().size() == 1);
        CHECK(new_task->has_child(child_task));
        CHECK(child_task->get_parent() == new_task);

        memfree(tree);
    }

    TEST_CASE("Test swap non root task in")
    {
        BehaviourTree* tree = memnew(BehaviourTree);
        godot::Ref<BTTask> old_task = memnew(BTTask);
        godot::Ref<BTTask> new_task = memnew(BTTask);
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);

        tree->add_task_by_ref(root_task);
        tree->add_task_by_ref(old_task);
        tree->add_task_by_ref(child1);
        tree->add_task_by_ref(child2);

        tree->connect_tasks(old_task, child1, 0);
        tree->connect_tasks(old_task, child2, 1);
        tree->connect_tasks(root_task, old_task, 0);

        tree->swap_task_in(old_task, new_task);

        CHECK(tree->has_task(new_task));
        CHECK_FALSE(tree->has_task(old_task));
        CHECK(tree->get_root_task() == root_task);
        CHECK(new_task->get_children().size() == 2);
        CHECK(new_task->has_child(child1));
        CHECK(new_task->has_child(child2));
        CHECK(child1->get_parent() == new_task);
        CHECK(child2->get_parent() == new_task);
        CHECK(new_task->get_parent() == root_task);
        CHECK(root_task->get_children().size() == 1);
        CHECK(root_task->has_child(new_task));

        memfree(tree);
    }

}