#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "tests/test_utils/test_utils.hpp"

#include "behaviour_tree/ui/bt_graph_sort_algorithm.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "tests/test_utils/signal_watcher.hpp"

struct BTGraphSortAlgorithmFixture
{
    godot::Ref<BTGraphSortAlgorithm> bt_sort_algorithm = memnew(BTGraphSortAlgorithm);
    godot::Vector<BTGraphNode*> graph_nodes = {};
    int initial_child_count = 0;

    BTGraphSortAlgorithmFixture()
    {}

    ~BTGraphSortAlgorithmFixture()
    {
        for (BTGraphNode* node : graph_nodes)
        {
            memdelete(node);
        }
    }

    BTGraphNode* create_graph_node()
    {
        BTGraphNode* node = memnew(BTGraphNode);
        this->graph_nodes.push_back(node);
        return node;
    }

    void check_neighbours(BTGraphNode* left, BTGraphNode* right)
    {
        REQUIRE(bt_sort_algorithm->right_neighbour.has(left));
        CHECK_EQ(bt_sort_algorithm->right_neighbour[left], right);

        REQUIRE(bt_sort_algorithm->left_neighbour.has(right));
        CHECK_EQ(bt_sort_algorithm->left_neighbour[right], left);
    }

    void check_parent(BTGraphNode* child, BTGraphNode* parent)
    {
        REQUIRE(bt_sort_algorithm->parent.has(child));
        CHECK_EQ(bt_sort_algorithm->parent[child], parent);
    }

};

TEST_SUITE("[editor]" "BTGraphSortAlgorithm")
{
    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Tree with only root")
    {
        BTGraphNode* root = create_graph_node();
        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;

        CHECK(bt_sort_algorithm->init_tree_utils(root, parent_to_children));
        CHECK(bt_sort_algorithm->left_neighbour.is_empty());
        CHECK(bt_sort_algorithm->right_neighbour.is_empty());
        CHECK(bt_sort_algorithm->parent.is_empty());
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Tree with root and two children")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* task1 = create_graph_node();
        BTGraphNode* task2 = create_graph_node();

        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;
        parent_to_children[root] = {task1, task2};

        CHECK(bt_sort_algorithm->init_tree_utils(root, parent_to_children));
        REQUIRE_EQ(bt_sort_algorithm->left_neighbour.size(), 1);
        REQUIRE_EQ(bt_sort_algorithm->right_neighbour.size(), bt_sort_algorithm->left_neighbour.size());
        REQUIRE_EQ(bt_sort_algorithm->parent.size(), 2);

        check_neighbours(task1, task2);
        check_parent(task1, root);
        check_parent(task2, root);
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Tree with multiple levels")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* task1 = create_graph_node();
        BTGraphNode* task2 = create_graph_node();
        BTGraphNode* task3 = create_graph_node();
        BTGraphNode* task11 = create_graph_node();
        BTGraphNode* task12 = create_graph_node();
        BTGraphNode* task31 = create_graph_node();
        BTGraphNode* task32 = create_graph_node();
        BTGraphNode* task33 = create_graph_node();

        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;
        parent_to_children[root] = {task1, task2, task3};
        parent_to_children[task1] = {task11, task12};
        parent_to_children[task3] = {task31, task32, task33};

        CHECK(bt_sort_algorithm->init_tree_utils(root, parent_to_children));
        REQUIRE_EQ(bt_sort_algorithm->left_neighbour.size(), 6);
        REQUIRE_EQ(bt_sort_algorithm->right_neighbour.size(), bt_sort_algorithm->left_neighbour.size());
        REQUIRE_EQ(bt_sort_algorithm->parent.size(), 8);

        check_neighbours(task1, task2);
        check_neighbours(task2, task3);
        check_neighbours(task11, task12);
        check_neighbours(task12, task31);
        check_neighbours(task31, task32);
        check_neighbours(task32, task33);

        check_parent(task1, root);
        check_parent(task2, root);
        check_parent(task3, root);
        check_parent(task11, task1);
        check_parent(task12, task1);
        check_parent(task31, task3);
        check_parent(task32, task3);
        check_parent(task33, task3);
    }

}

TEST_SUITE("[editor]" "[errors]" "BTGraphSortAlgorithm")
{
    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Fail to initialize tree utils with null root")
    {
        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;
        CHECK_THROWS(bt_sort_algorithm.init_tree_utils(nullptr, parent_to_children));
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Fail to initialize tree utils with null parent or child")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* valid_child = create_graph_node();

        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;
        parent_to_children[root] = {valid_child, nullptr};

        CHECK_THROWS(bt_sort_algorithm.init_tree_utils(root, parent_to_children));
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Fail to initialize tree utils with cyclic graph")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* child1 = create_graph_node();
        BTGraphNode* child2 = create_graph_node();

        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;
        parent_to_children[root] = {child1, child2};
        parent_to_children[child1] = {root}; // Creates a cycle

        CHECK_THROWS(bt_sort_algorithm.init_tree_utils(root, parent_to_children));
    }
}