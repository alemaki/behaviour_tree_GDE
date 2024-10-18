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

    BTGraphNode* root    = nullptr;
    BTGraphNode* task1   = nullptr;
    BTGraphNode* task11  = nullptr;
    BTGraphNode* task12  = nullptr;
    BTGraphNode* task13  = nullptr;
    BTGraphNode* task2   = nullptr;
    BTGraphNode* task21  = nullptr;
    BTGraphNode* task211 = nullptr;
    BTGraphNode* task212 = nullptr;
    BTGraphNode* task213 = nullptr;
    BTGraphNode* task214 = nullptr;
    BTGraphNode* task3   = nullptr;

    godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_children_names = {};

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

    void create_default_graph()
    {
        this->graph_view->create_task_node("root");
        this->graph_view->create_task_node("task1");
        this->graph_view->create_task_node("task11");
        this->graph_view->create_task_node("task12");
        this->graph_view->create_task_node("task13");
        this->graph_view->create_task_node("task2");
        this->graph_view->create_task_node("task21");
        this->graph_view->create_task_node("task211");
        this->graph_view->create_task_node("task212");
        this->graph_view->create_task_node("task213");
        this->graph_view->create_task_node("task214");
        this->graph_view->create_task_node("task3");
        this->root    = this->get_graph_node(0);
        this->task1   = this->get_graph_node(1);
        this->task11  = this->get_graph_node(2);
        this->task12  = this->get_graph_node(3);
        this->task13  = this->get_graph_node(4);
        this->task2   = this->get_graph_node(5);
        this->task21  = this->get_graph_node(6);
        this->task211 = this->get_graph_node(7);
        this->task212 = this->get_graph_node(8);
        this->task213 = this->get_graph_node(9);
        this->task214 = this->get_graph_node(10);
        this->task3   = this->get_graph_node(11);

        parent_to_children_names["root"] = {"task1", "task2", "task3"};
        parent_to_children_names["task1"] = {"task11", "task12", "task13"};
        parent_to_children_names["task2"] = {"task21"};
        parent_to_children_names["task21"] = {"task211", "task212", "task213", "task214"};
    }
};


TEST_SUITE("[editor]" "BTGraphView")
{
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

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test get_node")
    {
        create_default_graph();
        CHECK_EQ(graph_view->get_node_task("root"), root);
        CHECK_EQ(graph_view->get_node_task("task1"), task1);
        CHECK_EQ(graph_view->get_node_task("task11"), task11);
        CHECK_EQ(graph_view->get_node_task("task12"), task12);
        CHECK_EQ(graph_view->get_node_task("task2"), task2);
        CHECK_EQ(graph_view->get_node_task("task214"), task214);
        CHECK_EQ(graph_view->get_node_task("task212"), task212);
        CHECK_EQ(graph_view->get_node_task("task3"), task3);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test get_node_tree_map")
    {
        create_default_graph();
        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> result;

        result = graph_view->get_node_tree_map(parent_to_children_names);

        for (const godot::KeyValue<godot::StringName, godot::Vector<godot::StringName>>& key_value : parent_to_children_names)
        {
            BTGraphNode* parent = graph_view->get_node_task(key_value.key);
            REQUIRE_NE(parent, nullptr);
            REQUIRE(result.has(parent));
            for (const godot::StringName& child_name : key_value.value)
            {
                BTGraphNode* child = graph_view->get_node_task(child_name);
                CHECK_NE(child, nullptr);
                CHECK_EQ(result[parent].find(child), parent_to_children_names[key_value.key].find(child_name));
            }
        }
    }
}

TEST_SUITE("[editor]" "[errors]" "BTGraphView")
{
    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to create task with empty names")
    {
        CHECK_GODOT_ERROR(graph_view->create_task_node("", "TaskClass"));
        CHECK_GODOT_ERROR(graph_view->create_task_node("task_1", ""));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to set title of non-existent task")
    {
        CHECK_GODOT_ERROR(graph_view->set_task_title("non_existent", "Title"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to delete non-existent task node")
    {
        CHECK_GODOT_ERROR(graph_view->delete_task_node("non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to change class name of non-existent task")
    {
        CHECK_GODOT_ERROR(graph_view->change_task_class_name("non_existent", "NewClass"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to connect non-existent task nodes")
    {
        graph_view->create_task_node("task_1");

        CHECK_GODOT_ERROR(graph_view->connect_task_nodes("non_existent", "task_1"));
        CHECK_GODOT_ERROR(graph_view->connect_task_nodes("task_1", "non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to disconnect non-existent task nodes")
    {
        graph_view->create_task_node("task_1");

        CHECK_GODOT_ERROR(graph_view->disconnect_task_nodes("non_existent", "task_1"));
        CHECK_GODOT_ERROR(graph_view->disconnect_task_nodes("task_1", "non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to set position to non-existent task nodes")
    {
        CHECK_GODOT_ERROR(graph_view->set_node_position("non_existent", godot::Vector2(0, 0)));
    }
}