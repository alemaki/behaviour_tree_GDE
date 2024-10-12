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
}