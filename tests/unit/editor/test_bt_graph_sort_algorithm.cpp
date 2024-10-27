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
    godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children = {};
    godot::HashMap<BTGraphNode*, godot::Vector2> positions = {};

    /* for default graph */
    BTGraphNode* root = nullptr;
    BTGraphNode* child1 = nullptr;
    BTGraphNode* child2 = nullptr;
    BTGraphNode* child3 = nullptr;
    BTGraphNode* child11 = nullptr;
    BTGraphNode* child12 = nullptr;
    BTGraphNode* child31 = nullptr;
    BTGraphNode* child32 = nullptr;
    BTGraphNode* child33 = nullptr;

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


    void create_default_graph()
    {
        this->root = create_graph_node();
        this->child1 = create_graph_node();
        this->child2 = create_graph_node();
        this->child3 = create_graph_node();
        this->child11 = create_graph_node();
        this->child12 = create_graph_node();
        this->child31 = create_graph_node();
        this->child32 = create_graph_node();
        this->child33 = create_graph_node();

        parent_to_children[root] = {child1, child2, child3};
        parent_to_children[child1] = {child11, child12};
        parent_to_children[child3] = {child31, child32, child33};
        bt_sort_algorithm->set_parent_to_children(parent_to_children);
        bt_sort_algorithm->set_root_node(root);
    }

};

TEST_SUITE("[editor]" "BTGraphSortAlgorithm")
{
    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Tree with only root")
    {
        BTGraphNode* root = create_graph_node();
        bt_sort_algorithm->set_parent_to_children(parent_to_children);
        bt_sort_algorithm->set_root_node(root);

        REQUIRE(bt_sort_algorithm->init_tree_utils());
        CHECK(bt_sort_algorithm->left_neighbour.is_empty());
        CHECK(bt_sort_algorithm->right_neighbour.is_empty());
        CHECK(bt_sort_algorithm->parent.is_empty());
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Tree with root and two children")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* child1 = create_graph_node();
        BTGraphNode* child2 = create_graph_node();

        parent_to_children[root] = {child1, child2};
        bt_sort_algorithm->set_parent_to_children(parent_to_children);
        bt_sort_algorithm->set_root_node(root);

        REQUIRE(bt_sort_algorithm->init_tree_utils());
        REQUIRE_EQ(bt_sort_algorithm->left_neighbour.size(), 1);
        REQUIRE_EQ(bt_sort_algorithm->right_neighbour.size(), bt_sort_algorithm->left_neighbour.size());
        REQUIRE_EQ(bt_sort_algorithm->parent.size(), 2);

        check_neighbours(child1, child2);
        check_parent(child1, root);
        check_parent(child2, root);
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Tree with multiple levels")
    {
        create_default_graph();

        REQUIRE(bt_sort_algorithm->init_tree_utils());
        REQUIRE_EQ(bt_sort_algorithm->left_neighbour.size(), 6);
        REQUIRE_EQ(bt_sort_algorithm->right_neighbour.size(), bt_sort_algorithm->left_neighbour.size());
        REQUIRE_EQ(bt_sort_algorithm->parent.size(), 8);

        check_neighbours(child1, child2);
        check_neighbours(child2, child3);
        check_neighbours(child11, child12);
        check_neighbours(child12, child31);
        check_neighbours(child31, child32);
        check_neighbours(child32, child33);

        check_parent(child1, root);
        check_parent(child2, root);
        check_parent(child3, root);
        check_parent(child11, child1);
        check_parent(child12, child1);
        check_parent(child31, child3);
        check_parent(child32, child3);
        check_parent(child33, child3);
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "has_left_sibling and has_right_sibling")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* child1 = create_graph_node();
        BTGraphNode* child2 = create_graph_node();

        parent_to_children[root] = { child1, child2 };
        bt_sort_algorithm->set_parent_to_children(parent_to_children);
        bt_sort_algorithm->set_root_node(root);
        REQUIRE(bt_sort_algorithm->init_tree_utils());

        CHECK_FALSE(bt_sort_algorithm->has_left_sibling(root));
        CHECK_FALSE(bt_sort_algorithm->has_right_sibling(root));

        CHECK_FALSE(bt_sort_algorithm->has_left_sibling(child1));
        CHECK(bt_sort_algorithm->has_right_sibling(child1));

        CHECK(bt_sort_algorithm->has_left_sibling(child2));
        CHECK_FALSE(bt_sort_algorithm->has_right_sibling(child2));
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "get_leftmost returns correct leftmost descendant")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* child1 = create_graph_node();
        BTGraphNode* child2 = create_graph_node();
        BTGraphNode* child11 = create_graph_node();

        parent_to_children[root] = { child1, child2 };
        parent_to_children[child1] = { child11 };
        bt_sort_algorithm->set_parent_to_children(parent_to_children);
        bt_sort_algorithm->set_root_node(root);
        REQUIRE(bt_sort_algorithm->init_tree_utils());

        CHECK_EQ(bt_sort_algorithm->get_leftmost(root, 0, 1), child1);
        CHECK_EQ(bt_sort_algorithm->get_leftmost(child1, 0, 0), child1);
        CHECK_EQ(bt_sort_algorithm->get_leftmost(child2, 0, -1), child2);
        CHECK_EQ(bt_sort_algorithm->get_leftmost(root, 0, 2), child11);
        CHECK_EQ(bt_sort_algorithm->get_leftmost(child1, 0, 1), child11);
        CHECK_EQ( bt_sort_algorithm->get_leftmost(child2, 0, 0), child2);
        CHECK_EQ(bt_sort_algorithm->get_leftmost(root, 0, 3), nullptr);
        CHECK_EQ(bt_sort_algorithm->get_leftmost(child1, 0, 2), nullptr);
        CHECK_EQ( bt_sort_algorithm->get_leftmost(child2, 0, 1), nullptr);
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "apportion adjusts prelim and modifier")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* child1 = create_graph_node();
        BTGraphNode* child11 = create_graph_node();
        BTGraphNode* child12 = create_graph_node();
        BTGraphNode* child2 = create_graph_node();
        BTGraphNode* child21 = create_graph_node();
        BTGraphNode* child22 = create_graph_node();

        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children;
        parent_to_children[root] = { child1, child2 };
        parent_to_children[child1] = { child11, child12 };
        parent_to_children[child2] = { child21, child22 };
        bt_sort_algorithm->set_parent_to_children(parent_to_children);
        bt_sort_algorithm->set_root_node(root);
        REQUIRE(bt_sort_algorithm->init_tree_utils());

        int orig_prelim_child2 = bt_sort_algorithm->prelim[child2];

        REQUIRE(bt_sort_algorithm->apportion(child2, 1));

        /* Expect that child2's prelim is at least the left sibling's prelim
           plus the sibling_separation. */
        int expectedMin = bt_sort_algorithm->prelim[child1] + bt_sort_algorithm->sibling_separation;
        CHECK_GE(bt_sort_algorithm->prelim[child2], expectedMin);
        CHECK_GE(bt_sort_algorithm->modifier[child2], 0);
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "first_walk computes prelim for leaves and internals")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* child1 = create_graph_node();
        BTGraphNode* child2 = create_graph_node();
        BTGraphNode* grandchild1 = create_graph_node();

        parent_to_children[root] = { child1, child2 };
        parent_to_children[child1] = { grandchild1 };
        bt_sort_algorithm->set_parent_to_children(parent_to_children);
        bt_sort_algorithm->set_root_node(root);
        REQUIRE(bt_sort_algorithm->init_tree_utils());

        REQUIRE(bt_sort_algorithm->first_walk(root, 0));

        CHECK_EQ(bt_sort_algorithm->prelim[grandchild1], 0);

        /* For child2, which is a right sibling, its prelim should equal that of child1 plus
           the sibling_separation and half the sum of their sizes. */
        int expected = bt_sort_algorithm->prelim[child1] + bt_sort_algorithm->sibling_separation +
                       (child2->get_size().y + child1->get_size().y) / 2;

        CHECK_EQ(bt_sort_algorithm->prelim[child2], expected);
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Nodes are all acounted for.")
    {
        create_default_graph();
        positions = bt_sort_algorithm->get_arranged_nodes_position();

        for (BTGraphNode* node : graph_nodes)
        {
            CHECK(positions.has(node));
        }
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Nodes are appropriately spaced between levels")
    {
        create_default_graph();
        positions = bt_sort_algorithm->get_arranged_nodes_position();

        CHECK_EQ(positions[root].x, 0);
        CHECK_EQ(positions[child1].x, positions[child2].x);
        CHECK_EQ(positions[child1].x, positions[child3].x);
        CHECK_EQ(positions[child1].x, bt_sort_algorithm->level_separation);
        CHECK_EQ(positions[child11].x, positions[child12].x);
        CHECK_EQ(positions[child11].x, positions[child12].x);
        CHECK_EQ(positions[child11].x, positions[child31].x);
        CHECK_EQ(positions[child11].x, positions[child32].x);
        CHECK_EQ(positions[child11].x, positions[child33].x);
        CHECK_EQ(positions[child11].x, 2 * bt_sort_algorithm->level_separation);
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Sibling nodes are properly spaced to avoid overlap")
    {
        create_default_graph();
        positions = bt_sort_algorithm->get_arranged_nodes_position();
        
        int min_separation = bt_sort_algorithm->sibling_separation;
        godot::Vector<godot::Vector<BTGraphNode*>> node_levels = {
            {root}, 
            {child1, child2, child3},
            {child11, child12, child31, child32, child33}
        };

        for (const godot::Vector<BTGraphNode*>& level : node_levels)
        {
            for (int i = 0; i < level.size() - 1; i++)
            {
                CHECK_GE(abs(positions[level[i + 1]].y - positions[level[i]].y), min_separation);
            }
        }
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Parents are centered between their children")
    {
        create_default_graph();
        positions = bt_sort_algorithm->get_arranged_nodes_position();
        
        int expected_center1 = (positions[child11].y + positions[child12].y) / 2;
        CHECK_EQ(positions[child1].y, expected_center1);

        int expected_center3 = (positions[child31].y + positions[child32].y + positions[child33].y) / 3;
        CHECK_EQ(positions[child3].y, expected_center3);
        CHECK_EQ(positions[child3].y, positions[child32].y);
    }

}

TEST_SUITE("[editor]" "[errors]" "BTGraphSortAlgorithm")
{
    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Fail to initialize tree utils with null root")
    {
        CHECK_GODOT_ERROR(bt_sort_algorithm->init_tree_utils());
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Fail to initialize tree utils with null parent or child")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* valid_child = create_graph_node();

        parent_to_children[root] = {valid_child, nullptr};

        bt_sort_algorithm->set_root_node(root);
        bt_sort_algorithm->set_parent_to_children(parent_to_children);

        CHECK_GODOT_ERROR(bt_sort_algorithm->init_tree_utils());
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "Fail to initialize tree utils with cyclic graph")
    {
        BTGraphNode* root = create_graph_node();
        BTGraphNode* child1 = create_graph_node();
        BTGraphNode* child2 = create_graph_node();

        parent_to_children[root] = {child1, child2};
        parent_to_children[child1] = {root}; // Creates a cycle

        bt_sort_algorithm->set_root_node(root);
        bt_sort_algorithm->set_parent_to_children(parent_to_children);

        CHECK_GODOT_ERROR(bt_sort_algorithm->init_tree_utils());
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "has_left_sibling and has_right_sibling should fail on nullptr")
    {
        CHECK_GODOT_ERROR(bt_sort_algorithm->has_left_sibling(nullptr));
        CHECK_GODOT_ERROR(bt_sort_algorithm->has_right_sibling(nullptr));
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "apportion should fail on nullptr node")
    {
        CHECK_GODOT_ERROR(bt_sort_algorithm->apportion(nullptr, 0));
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "apportion should fail on a node with no parent")
    {
        BTGraphNode* orphan = create_graph_node();
        bt_sort_algorithm->set_root_node(orphan);
        REQUIRE(bt_sort_algorithm->init_tree_utils());
        CHECK_GODOT_ERROR(bt_sort_algorithm->apportion(orphan, 0));
    }

    TEST_CASE_FIXTURE(BTGraphSortAlgorithmFixture, "first_walk should fail on nullptr")
    {
        CHECK_GODOT_ERROR(bt_sort_algorithm->first_walk(nullptr, 0));
    }
}