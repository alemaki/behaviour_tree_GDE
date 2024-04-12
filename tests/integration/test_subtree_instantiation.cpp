#include "doctest.h"
#include <godot_cpp/classes/resource_loader.hpp>


#include "behaviour_tree/behaviour_tree.hpp"
#include "behaviour_tree/tasks/bt_subtree.hpp"
#include "behaviour_tree/tasks/bt_task.hpp"


TEST_SUITE("SubtreeInstantioationTest")
{
    BehaviourTree* mock_behaviour_tree = nullptr;
    BehaviourTree* mock_behaviour_tree_with_sub = nullptr;

    TEST_CASE("Setup")
    {
        const godot::String mock_behaviour_tree_path = "res://tests/mock/mock_behaviour_tree.tscn";
        const godot::String mock_behaviour_tree_with_sub_path = "res://tests/mock/mock_behaviour_tree_with_sub.tscn";


        godot::Node* mock_behaviour_tree_node = godot::Ref<godot::PackedScene>(godot::ResourceLoader::get_singleton()->load(mock_behaviour_tree_path))->instantiate();
        mock_behaviour_tree = godot::Object::cast_to<BehaviourTree>(mock_behaviour_tree_node);

        godot::Node* mock_behaviour_tree_with_sub_node = godot::Ref<godot::PackedScene>(godot::ResourceLoader::get_singleton()->load(mock_behaviour_tree_with_sub_path))->instantiate();
        mock_behaviour_tree_with_sub = godot::Object::cast_to<BehaviourTree>(mock_behaviour_tree_with_sub_node);

        REQUIRE(mock_behaviour_tree != nullptr);
        REQUIRE(mock_behaviour_tree_with_sub != nullptr);
    }

    TEST_CASE("Test mock tree valid for testing")
    {
        REQUIRE(mock_behaviour_tree != nullptr);
        REQUIRE(mock_behaviour_tree->get_root_task() != nullptr);
        REQUIRE(mock_behaviour_tree->get_root_task()->get_child_count() == 2);
        REQUIRE(mock_behaviour_tree->get_root_task()->get_child(0)->get_child_count() == 0);
        REQUIRE(mock_behaviour_tree->get_root_task()->get_child(1)->get_child_count() == 0);
    }

    TEST_CASE("Test mock tree with subtree valid for testing")
    {
        const godot::String mock_behaviour_tree_path = "res://tests/mock/mock_behaviour_tree.tscn";

        REQUIRE(mock_behaviour_tree_with_sub != nullptr);
        REQUIRE(mock_behaviour_tree_with_sub->get_root_task() != nullptr);
        REQUIRE(mock_behaviour_tree_with_sub->get_root_task()->get_child_count() == 1);
        REQUIRE(mock_behaviour_tree_with_sub->get_root_task()->get_child(0)->get_child_count() == 1);
        REQUIRE(mock_behaviour_tree_with_sub->get_root_task()->get_child(0)->get_child(0)->get_class() == godot::String("BTSubtree"));
        REQUIRE(godot::Ref<BTSubtree>(mock_behaviour_tree_with_sub->get_root_task()->get_child(0)->get_child(0))->get_file_path() == mock_behaviour_tree_path);
    }

    TEST_CASE("Test load behavior tree")
    {
        const godot::String mock_behaviour_tree_path = "res://tests/mock/mock_behaviour_tree.tscn";

        godot::Ref<BTSubtree> subtree_task = memnew(BTSubtree);
        subtree_task->set_file_path(mock_behaviour_tree_path);

        BehaviourTree* sub_behaviour_tree = subtree_task->load_behavior_tree();

        REQUIRE(sub_behaviour_tree != nullptr);
        CHECK(sub_behaviour_tree->get_root_task() != nullptr);

        memfree(sub_behaviour_tree);
    }

    TEST_CASE("Test clone subtree task")
    {
        const godot::String mock_behaviour_tree_path = "res://tests/mock/mock_behaviour_tree.tscn";
        
        godot::Ref<BTSubtree> subtree_task = memnew(BTSubtree);
        subtree_task->set_file_path(mock_behaviour_tree_path);

        godot::Ref<BTTask> tree_root_task = mock_behaviour_tree->get_root_task();
        godot::Ref<BTTask> cloned_root_task = subtree_task->clone();

        REQUIRE(cloned_root_task != nullptr);
        CHECK(cloned_root_task != tree_root_task);
        REQUIRE(tree_root_task->get_child_count() == cloned_root_task->get_child_count());
        CHECK(tree_root_task->get_class() == cloned_root_task->get_class());

        godot::Ref<BTTask> child1 = tree_root_task->get_child(0);
        godot::Ref<BTTask> child2 = tree_root_task->get_child(1);
        godot::Ref<BTTask> cloned_child1 = cloned_root_task->get_child(0);
        godot::Ref<BTTask> cloned_child2 = cloned_root_task->get_child(1);

        CHECK(child1 != cloned_child1);
        CHECK(child1->get_child_count() == cloned_child1->get_child_count());
        CHECK(child1->get_class() == cloned_child1->get_class());

        CHECK(child2 != cloned_child2);
        CHECK(child2->get_child_count() == cloned_child2->get_child_count());
        CHECK(child2->get_class() == cloned_child2->get_class());
    }

    TEST_CASE("Test clone subtree with subtree")
    {
        const godot::String mock_behaviour_tree_with_sub_path = "res://tests/mock/mock_behaviour_tree_with_sub.tscn";

        godot::Ref<BTSubtree> subtree_task = memnew(BTSubtree);
        subtree_task->set_file_path(mock_behaviour_tree_with_sub_path);

        godot::Ref<BTTask> subtree_root_task = mock_behaviour_tree->get_root_task();
        godot::Ref<BTTask> tree_root_task = mock_behaviour_tree_with_sub->get_root_task();
        godot::Ref<BTTask> cloned_root_task = subtree_task->clone();

        REQUIRE(cloned_root_task != nullptr);
        CHECK(cloned_root_task != tree_root_task);
        REQUIRE(tree_root_task->get_child_count() == cloned_root_task->get_child_count());
        CHECK(tree_root_task->get_class() == cloned_root_task->get_class());

        godot::Ref<BTTask> child1 = tree_root_task->get_child(0);
        godot::Ref<BTTask> cloned_child1 = cloned_root_task->get_child(0);

        CHECK(child1 != cloned_child1);
        CHECK(child1->get_child_count() == cloned_child1->get_child_count());
        CHECK(child1->get_class() == cloned_child1->get_class());

        godot::Ref<BTTask> cloned_child_subtree = cloned_root_task->get_child(0)->get_child(0);
        CHECK(cloned_child_subtree != subtree_root_task);
        REQUIRE(cloned_child_subtree->get_child_count() == subtree_root_task->get_child_count());
        CHECK(cloned_child_subtree->get_class() == subtree_root_task->get_class());

        godot::Ref<BTTask> leaf1 = subtree_root_task->get_child(0);
        godot::Ref<BTTask> leaf2 = subtree_root_task->get_child(1);
        godot::Ref<BTTask> cloned_leaf1 = cloned_root_task->get_child(0)->get_child(0)->get_child(0);
        godot::Ref<BTTask> cloned_leaf2 = cloned_root_task->get_child(0)->get_child(0)->get_child(1);

        CHECK(leaf1 != cloned_leaf1);
        CHECK(leaf1->get_child_count() == cloned_leaf1->get_child_count());
        CHECK(leaf1->get_class() == cloned_leaf1->get_class());

        CHECK(leaf2 != cloned_leaf2);
        CHECK(leaf2->get_child_count() == cloned_leaf2->get_child_count());
        CHECK(leaf2->get_class() == cloned_leaf2->get_class());
    }

    TEST_CASE("Teardown")
    {
        memfree(mock_behaviour_tree);
        memfree(mock_behaviour_tree_with_sub);
    }
}