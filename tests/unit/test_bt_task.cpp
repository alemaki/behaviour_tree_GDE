#include "doctest.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"
#include "behaviour_tree/tasks/decorators/bt_invert.hpp"
#include "behaviour_tree/tasks/decorators/bt_probability.hpp"
#include "behaviour_tree/tasks/composites/bt_sequence.hpp"


TEST_SUITE("BTTaskTests")
{

    TEST_CASE("BTTask Basic Functionality")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        CHECK(task->get_status() == 0);

        godot::Node* actor_node = memnew(godot::Node);
        task->set_actor(actor_node);
        CHECK(task->get_actor() == actor_node);

        CHECK(task->get_parent() == nullptr);
    }

    TEST_CASE("BTTask Child Addition and Array")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);
        godot::Ref<BTTask> child3 = memnew(BTTask);

        root_task->add_child(child1);
        root_task->add_child(child2);

        godot::Array children = root_task->get_children();
        CHECK(children[0] == child1);
        CHECK(children[1] == child2);

        root_task->add_child(child3);

        children = root_task->get_children();
        CHECK(children[0] == child1);
        CHECK(children[1] == child2);
        CHECK(children[2] == child3);
    }

    TEST_CASE("BTTask Has Child")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child_task = memnew(BTTask);

        CHECK_FALSE(root_task->has_child(child_task));
        root_task->add_child(child_task);
        CHECK(root_task->has_child(child_task));

        godot::Ref<BTTask> other_task = memnew(BTTask);
        CHECK_FALSE(root_task->has_child(other_task));
    }

    TEST_CASE("BTTask Child Count")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);
        godot::Ref<BTTask> child3 = memnew(BTTask);

        root_task->add_child(child1);
        CHECK(root_task->get_child_count() == 1);

        root_task->add_child(child2);
        CHECK(root_task->get_child_count() == 2);

        root_task->add_child(child3);
        CHECK(root_task->get_child_count() == 3);
    }

    TEST_CASE("BTTask Child Twice Addition")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);

        root_task->add_child(child1);
        root_task->add_child(child2);
        root_task->add_child(child1);
        root_task->add_child(child2);
        CHECK(root_task->get_child_count() == 2);
    }

    TEST_CASE("BTTask Child Addition at Index")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);
        godot::Ref<BTTask> child3 = memnew(BTTask);

        root_task->add_child(child1);
        root_task->add_child_at_index(child2, 0);

        godot::Array children = root_task->get_children();
        CHECK(children[0] == child2);
        CHECK(children[1] == child1);

        root_task->add_child_at_index(child3, 1);

        children = root_task->get_children();
        CHECK(children[0] == child2);
        CHECK(children[1] == child3);
        CHECK(children[2] == child1);
    }

    TEST_CASE("BTTask Child Removal")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);
        godot::Ref<BTTask> child3 = memnew(BTTask);

        root_task->add_child(child1);
        root_task->add_child(child2);
        root_task->add_child(child3);

        root_task->remove_child(child1);
        CHECK_FALSE(root_task->has_child(child1));
        CHECK(root_task->get_child_count() == 2);
        CHECK(child1->get_parent() == nullptr);

        godot::Array children = root_task->get_children();
        CHECK(children[0] == child2);
        CHECK(children[1] == child3);

        root_task->remove_child(child3);
        CHECK_FALSE(root_task->has_child(child3));
        CHECK(child3->get_parent() == nullptr);
        CHECK(root_task->has_child(child2));
    }

    TEST_CASE("BTTask Child Twice Removal")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);

        root_task->add_child(child1);
        root_task->add_child(child2);
        CHECK(root_task->get_child_count() == 2);

        root_task->remove_child(child1);
        root_task->remove_child(child1);
        CHECK(root_task->get_child_count() == 1);
    }

    TEST_CASE("BTTask Child Removal at Index")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child1 = memnew(BTTask);
        godot::Ref<BTTask> child2 = memnew(BTTask);
        godot::Ref<BTTask> child3 = memnew(BTTask);

        root_task->add_child(child1);
        root_task->add_child(child2);
        root_task->add_child(child3);

        root_task->remove_child_at_index(1);
        CHECK_FALSE(root_task->has_child(child2));
        CHECK(child2->get_parent() == nullptr);
        CHECK(root_task->get_child_count() == 2);

        godot::Array children = root_task->get_children();
        CHECK(children[0] == child1);
        CHECK(children[1] == child3);

        root_task->remove_child_at_index(0);
        CHECK_FALSE(root_task->has_child(child1));
        CHECK(child1->get_parent() == nullptr);
    }

    TEST_CASE("BTTask Get Parent After Children Added")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child_task = memnew(BTTask);
        godot::Ref<BTTask> grandchild_task = memnew(BTTask);

        root_task->add_child(child_task);
        child_task->add_child(grandchild_task);

        CHECK(grandchild_task->get_parent() == child_task);

        godot::Ref<BTTask> another_child_task = memnew(BTTask);
        root_task->add_child(another_child_task);
        CHECK(another_child_task->get_parent() == root_task);
        CHECK(child_task->get_parent() == root_task);
    }

    TEST_CASE("BTTask Is Root")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child_task = memnew(BTTask);
        godot::Ref<BTTask> grandchild_task = memnew(BTTask);

        root_task->add_child(child_task);
        child_task->add_child(grandchild_task);

        CHECK(root_task->is_root());
        CHECK_FALSE(child_task->is_root());
        CHECK_FALSE(grandchild_task->is_root());
    }

    TEST_CASE("BTTask Get Root")
    {
        godot::Ref<BTTask> root_task = memnew(BTTask);
        godot::Ref<BTTask> child_task = memnew(BTTask);
        godot::Ref<BTTask> grandchild_task = memnew(BTTask);

        root_task->add_child(child_task);
        child_task->add_child(grandchild_task);

        CHECK(grandchild_task->get_root() == root_task);
        CHECK(child_task->get_root() == root_task);
    }

    TEST_CASE("BTTask Clone Basic")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        godot::Ref<BTTask> cloned_task = task->clone();

        CHECK(cloned_task != nullptr);
        CHECK(cloned_task != task);
        CHECK(cloned_task->get_class() == task->get_class());
        CHECK(cloned_task->get_actor() == task->get_actor());
        CHECK(cloned_task->get_status() == task->get_status());
    }

    TEST_CASE("BTTask Clone Keeps Attributes")
    {
        godot::Ref<BTProbability> task_prob = memnew(BTProbability);
        task_prob->set_run_chance(0.5);
        godot::Ref<BTTask> cloned_task = task_prob->clone();

        CHECK(cloned_task != task_prob);
        CHECK(cloned_task->get_class() == task_prob->get_class());
        CHECK(static_cast<BTProbability*>(cloned_task.ptr())->get_run_chance() == task_prob->get_run_chance());
    }

    TEST_CASE("BTTask Cloning Children")
    {
        godot::Ref<BTTask> task = memnew(BTTask);
        godot::Ref<BTInvert> child1 = memnew(BTInvert);
        godot::Ref<BTSequence> child2 = memnew(BTSequence);

        task->add_child(child1);
        task->add_child(child2);

        godot::Ref<BTTask> cloned_task = task->clone();

        CHECK(cloned_task != task);
        CHECK(cloned_task->get_child_count() == 2);
        CHECK_FALSE(cloned_task->has_child(child1));
        CHECK_FALSE(cloned_task->has_child(child2));

        godot::Array cloned_children = cloned_task->get_children();
        CHECK(godot::Ref<BTTask>(cloned_children[0])->get_class() == child1->get_class());
        CHECK(godot::Ref<BTTask>(cloned_children[1])->get_class() == child2->get_class());
    }
}