#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "tests/test_utils/test_utils.hpp"

#include "behaviour_tree/ui/bt_graph_view.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"
#include "tests/test_utils/signal_watcher.hpp"

struct BTGraphViewFixture
{
    BTGraphView* graph_view = memnew(BTGraphView);
    BTGraphView::TreeArrangeUtils utils;
    int initial_child_count = 0;
    BTGraphViewFixture()
    {
        this->initial_child_count = graph_view->get_child_count();
    }

    BTGraphNode* get_graph_node(int index) const
    {
        return godot::Object::cast_to<BTGraphNode>(graph_view->get_child(initial_child_count + index));
    }

    ~BTGraphViewFixture()
    {
        memdelete(graph_view);
    }

    void check_neighbours(BTGraphNode* left, BTGraphNode* right) {
        REQUIRE(utils.right_neighbour.has(left));
        CHECK_EQ(utils.right_neighbour[left], right);
        
        REQUIRE(utils.left_neighbour.has(right));
        CHECK_EQ(utils.left_neighbour[right], left);
    }
    
    void check_parent(BTGraphNode* child, BTGraphNode* parent) {
        REQUIRE(utils.parent.has(child));
        CHECK_EQ(utils.parent[child], parent);
    }
};


TEST_SUITE("[editor]" "BTGraphView")
{
    TEST_CASE_FIXTURE(BTGraphViewFixture, "Setters and getters")
    {
        REQUIRE_EQ(graph_view->get_undo_redo_manager(), nullptr);
        REQUIRE_FALSE(graph_view->is_undo_redo_enabled());
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Create task node")
    {
        graph_view->create_task_node("task_1");

        REQUIRE(graph_view->has_task_name("task_1"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Set task title")
    {
        graph_view->create_task_node("task_1");

        graph_view->set_task_title("task_1", "Task Title");
        BTGraphNode* graph_node = this->get_graph_node(0);
        REQUIRE_EQ(graph_node->get_title(), godot::StringName("Task Title"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Delete task node")
    {
        graph_view->create_task_node("task_1");

        graph_view->delete_task_node("task_1");

        REQUIRE_FALSE(graph_view->has_task_name("task_1"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Change task class name")
    {
        graph_view->create_task_node("task_1");

        graph_view->change_task_class_name("task_1", "NewClass");
        BTGraphNode* graph_node = this->get_graph_node(0);
        REQUIRE_EQ(graph_node->get_task_class_name(), godot::StringName("NewClass"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Connect task nodes")
    {
        graph_view->create_task_node("parent_task");
        graph_view->create_task_node("child_task");

        graph_view->connect_task_nodes("parent_task", "child_task");

        BTGraphNode* parent_node = this->get_graph_node(0);
        BTGraphNode* child_node = this->get_graph_node(1);

        REQUIRE(graph_view->is_node_connected(parent_node->get_name(), 0, child_node->get_name(), 0));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Disconnect task nodes")
    {
        graph_view->create_task_node("parent_task");
        graph_view->create_task_node("child_task");

        graph_view->connect_task_nodes("parent_task", "child_task");
        graph_view->disconnect_task_nodes("parent_task", "child_task");

        BTGraphNode* parent_node = this->get_graph_node(0);
        BTGraphNode* child_node = this->get_graph_node(1);

        REQUIRE_FALSE(graph_view->is_node_connected(parent_node->get_name(), 0, child_node->get_name(), 0));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Moving task nodes")
    {
        graph_view->create_task_node("task");
        BTGraphNode* graph_node = this->get_graph_node(0);

        graph_view->set_node_position("task", godot::Vector2(0, 0));
        CHECK_VECTORS_EQ(graph_node->get_position_offset(), godot::Vector2(0, 0));

        graph_view->set_node_position("task", godot::Vector2(0, 1));
        CHECK_VECTORS_EQ(graph_node->get_position_offset(), godot::Vector2(0, 1));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Tree with only root")
    {
        graph_view->create_task_node("root");
        godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_children_names;

        CHECK(graph_view->init_tree_utils(utils, "root", parent_to_children_names));
        CHECK(utils.left_neighbour.is_empty());
        CHECK(utils.right_neighbour.is_empty());
        CHECK(utils.parent.is_empty());
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Tree with root and two children")
    {
        graph_view->create_task_node("root");
        godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_children_names;
        graph_view->create_task_node("task1");
        graph_view->create_task_node("task2");
        parent_to_children_names["root"] = {"task1", "task2"};

        BTGraphNode* root = get_graph_node(0);
        BTGraphNode* task1 = get_graph_node(1);
        BTGraphNode* task2 = get_graph_node(2);

        CHECK(graph_view->init_tree_utils(utils, "root", parent_to_children_names));
        REQUIRE_EQ(utils.left_neighbour.size(), 1);
        REQUIRE_EQ(utils.right_neighbour.size(), utils.left_neighbour.size());
        REQUIRE_EQ(utils.parent.size(), 2);

        this->check_neighbours(task1, task2);
        this->check_parent(task1, root);
        this->check_parent(task2, root);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Tree with root and two children")
    {
        godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_children_names;
        graph_view->create_task_node("root");
        graph_view->create_task_node("task1");
        graph_view->create_task_node("task2");

        BTGraphNode* root = get_graph_node(0);
        BTGraphNode* task1 = get_graph_node(1);
        BTGraphNode* task2 = get_graph_node(2);

        graph_view->create_task_node("task3");
        graph_view->create_task_node("task11");
        graph_view->create_task_node("task12");
        graph_view->create_task_node("task31");
        graph_view->create_task_node("task32");
        graph_view->create_task_node("task33");
        
        parent_to_children_names["root"] = {"task1", "task2", "task3"};
        parent_to_children_names["task1"] = {"task11", "task12"};
        parent_to_children_names["task3"] = {"task31", "task32", "task33"};

        BTGraphNode* task3 = get_graph_node(3);
        BTGraphNode* task11 = get_graph_node(4);
        BTGraphNode* task12 = get_graph_node(5);
        BTGraphNode* task31 = get_graph_node(6);
        BTGraphNode* task32 = get_graph_node(7);
        BTGraphNode* task33 = get_graph_node(8);

        CHECK(graph_view->init_tree_utils(utils, "root", parent_to_children_names));
        REQUIRE_EQ(utils.left_neighbour.size(), 6);
        REQUIRE_EQ(utils.right_neighbour.size(), utils.left_neighbour.size());
        REQUIRE_EQ(utils.parent.size(), 8);

        this->check_neighbours(task1, task2);
        this->check_neighbours(task2, task3);
        this->check_neighbours(task11, task12);
        this->check_neighbours(task12, task31);
        this->check_neighbours(task31, task32);
        this->check_neighbours(task32, task33);

        this->check_parent(task1, root);
        this->check_parent(task2, root);
        this->check_parent(task3, root);
        this->check_parent(task11, task1);
        this->check_parent(task12, task1);
        this->check_parent(task31, task3);
        this->check_parent(task32, task3);
        this->check_parent(task33, task3);
    }
}

TEST_SUITE("[editor]" "[errors]" "BTGraphView")
{
    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to create task with empty names")
    {
        CHECK_THROWS(graph_view->create_task_node("", "TaskClass"));
        CHECK_THROWS(graph_view->create_task_node("task_1", ""));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to set title of non-existent task")
    {
        CHECK_THROWS(graph_view->set_task_title("non_existent", "Title"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to delete non-existent task node")
    {
        CHECK_THROWS(graph_view->delete_task_node("non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to change class name of non-existent task")
    {
        CHECK_THROWS(graph_view->change_task_class_name("non_existent", "NewClass"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to connect non-existent task nodes")
    {
        graph_view->create_task_node("task_1");

        CHECK_THROWS(graph_view->connect_task_nodes("non_existent", "task_1"));
        CHECK_THROWS(graph_view->connect_task_nodes("task_1", "non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to disconnect non-existent task nodes")
    {
        graph_view->create_task_node("task_1");

        CHECK_THROWS(graph_view->disconnect_task_nodes("non_existent", "task_1"));
        CHECK_THROWS(graph_view->disconnect_task_nodes("task_1", "non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to set position to non-existent task nodes")
    {
        CHECK_THROWS(graph_view->set_node_position("non_existent", godot::Vector2(0, 0)));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to initialize tree utils with non-existent root")
    {
        BTGraphView::TreeArrangeUtils utils;
        godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_children_names;
        CHECK_THROWS(graph_view->init_tree_utils(utils, "non_existent", parent_to_children_names));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to initialize tree utils with non-existent parent or child")
    {
        BTGraphView::TreeArrangeUtils utils;
        graph_view->create_task_node("root");
        godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_children_names;
        parent_to_children_names["root"] = godot::Vector<godot::StringName>({"non_existent"});
        CHECK_THROWS(graph_view->init_tree_utils(utils, "root", parent_to_children_names));
    }
}