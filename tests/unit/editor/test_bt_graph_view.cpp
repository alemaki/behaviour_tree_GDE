#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

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
        graph_view->create_task_node("task_1", "TaskClass");

        REQUIRE(graph_view->has_task_name("task_1"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Set task title")
    {
        graph_view->create_task_node("task_1", "TaskClass");

        graph_view->set_task_title("task_1", "Task Title");
        BTGraphNode* graph_node = this->get_graph_node(0);
        REQUIRE_EQ(graph_node->get_title(), godot::StringName("Task Title"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Delete task node")
    {
        graph_view->create_task_node("task_1", "TaskClass");

        graph_view->delete_task_node("task_1");

        REQUIRE_FALSE(graph_view->has_task_name("task_1"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Change task class name")
    {
        graph_view->create_task_node("task_1", "InitialClass");

        graph_view->change_task_class_name("task_1", "NewClass");
        BTGraphNode* graph_node = this->get_graph_node(0);
        REQUIRE_EQ(graph_node->get_task_class_name(), godot::StringName("NewClass"));
    }

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
}