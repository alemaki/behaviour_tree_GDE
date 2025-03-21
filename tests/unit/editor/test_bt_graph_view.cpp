#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#include "tests/test_utils/test_macros.hpp"

#include "behaviour_tree/ui/bt_graph_view.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"

struct BTGraphViewFixture
{
    BTGraphView* graph_view = memnew(BTGraphView);
    int initial_child_count = 0;
    godot::Vector<StringName> default_task_names = {};
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
        this->graph_view->connect_task_nodes("root", "task1");
        this->graph_view->connect_task_nodes("root", "task2");
        this->graph_view->connect_task_nodes("root", "task3");
        this->graph_view->connect_task_nodes("task1", "task11");
        this->graph_view->connect_task_nodes("task1", "task12");
        this->graph_view->connect_task_nodes("task1", "task13");
        this->graph_view->connect_task_nodes("task2", "task21");
        this->graph_view->connect_task_nodes("task21", "task211");
        this->graph_view->connect_task_nodes("task21", "task212");
        this->graph_view->connect_task_nodes("task21", "task213");
        this->graph_view->connect_task_nodes("task21", "task214");
        this->parent_to_children_names["root"] = {"task1", "task2", "task3"};
        this->parent_to_children_names["task1"] = {"task11", "task12", "task13"};
        this->parent_to_children_names["task2"] = {"task21"};
        this->parent_to_children_names["task21"] = {"task211", "task212", "task213", "task214"};
        this->default_task_names = {"root", "task1", "task2", "task3", "task11", "task12", "task13", "task21", "task211", "task212", "task213", "task214"};
    }
};


TEST_SUITE("[editor]" "BTGraphView")
{
    TEST_CASE_FIXTURE(BTGraphViewFixture, "Create task node")
    {
        graph_view->create_task_node("task_1");

        REQUIRE(graph_view->has_task_name("task_1"));
        CHECK_EQ(graph_view->get_child_count(), initial_child_count + 1);

        graph_view->create_task_node("task_2");
        REQUIRE(graph_view->has_task_name("task_2"));
        CHECK_EQ(graph_view->get_child_count(), initial_child_count + 2);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Set task title")
    {
        graph_view->create_task_node("task_1");

        graph_view->set_task_node_title("task_1", "Task Title");
        BTGraphNode* graph_node = this->get_graph_node(0);
        REQUIRE_EQ(graph_node->get_title(), godot::StringName("Task Title"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Delete task node")
    {
        graph_view->create_task_node("task_1");
        graph_view->create_task_node("task_2");

        graph_view->delete_task_node("task_1");

        REQUIRE_FALSE(graph_view->has_task_name("task_1"));
        REQUIRE(graph_view->has_task_name("task_2"));
        CHECK_EQ(graph_view->get_child_count(), initial_child_count + 1);

        graph_view->delete_task_node("task_2");
        REQUIRE_FALSE(graph_view->has_task_name("task_2"));
        CHECK_EQ(graph_view->get_child_count(), initial_child_count);
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

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test get node")
    {
        create_default_graph();
        CHECK_EQ(graph_view->get_graph_node("root"), root);
        CHECK_EQ(graph_view->get_graph_node("root"), root);
        CHECK_EQ(graph_view->get_graph_node("task1"), task1);
        CHECK_EQ(graph_view->get_graph_node("task11"), task11);
        CHECK_EQ(graph_view->get_graph_node("task12"), task12);
        CHECK_EQ(graph_view->get_graph_node("task2"), task2);
        CHECK_EQ(graph_view->get_graph_node("task214"), task214);
        CHECK_EQ(graph_view->get_graph_node("task212"), task212);
        CHECK_EQ(graph_view->get_graph_node("task3"), task3);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test get task name")
    {
        create_default_graph();
        CHECK_EQ(graph_view->get_task_name(root->get_name()), godot::String("root"));
        CHECK_EQ(graph_view->get_task_name(task1->get_name()), godot::String("task1"));
        CHECK_EQ(graph_view->get_task_name(task12->get_name()), godot::String("task12"));
        CHECK_EQ(graph_view->get_task_name(task2->get_name()), godot::String("task2"));
        CHECK_EQ(graph_view->get_task_name(task214->get_name()), godot::String("task214"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test get node tree map")
    {
        create_default_graph();
        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> result;

        result = graph_view->get_node_tree_map(parent_to_children_names);

        for (const godot::KeyValue<godot::StringName, godot::Vector<godot::StringName>>& key_value : parent_to_children_names)
        {
            BTGraphNode* parent = graph_view->get_graph_node(key_value.key);
            REQUIRE_NE(parent, nullptr);
            REQUIRE(result.has(parent));
            for (const godot::StringName& child_name : key_value.value)
            {
                BTGraphNode* child = graph_view->get_graph_node(child_name);
                CHECK_NE(child, nullptr);
                CHECK_EQ(result[parent].find(child), parent_to_children_names[key_value.key].find(child_name));
            }
        }
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test arrange nodes")
    {
        create_default_graph();
        godot::Ref<BTGraphSortAlgorithm> graph_sort_algorithm = memnew(BTGraphSortAlgorithm);
        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children = graph_view->get_node_tree_map(parent_to_children_names);

        graph_sort_algorithm->set_root_node(root);
        graph_sort_algorithm->set_parent_to_children(parent_to_children);

        godot::HashMap<BTGraphNode*, godot::Vector2> result = graph_sort_algorithm->get_arranged_nodes_position();

        graph_view->arrange_nodes("root", parent_to_children_names);

        for (const godot::KeyValue<BTGraphNode*, godot::Vector2>& key_value : result)
        {
            CHECK_VECTORS_EQ(key_value.key->get_position_offset(), key_value.value);
        }
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test save graph")
    {
        create_default_graph();
        graph_view->clear_and_save_graph("graph1");
        create_default_graph();
        graph_view->clear_and_save_graph("graph2");

        REQUIRE(graph_view->has_saved_graph("graph1"));
        REQUIRE(graph_view->has_saved_graph("graph2"));
        
        for (const godot::StringName& task_name : default_task_names)
        {
            CHECK_FALSE(graph_view->has_task_name(task_name));
        }
        REQUIRE(graph_view->get_child_count() == initial_child_count);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Test load graph")
    {
        create_default_graph();
        graph_view->clear_and_save_graph("graph");
        graph_view->load_graph("graph");
        REQUIRE_FALSE(graph_view->has_saved_graph("graph"));
        for (const godot::StringName& task_name : default_task_names)
        {
            CHECK(graph_view->has_task_name(task_name));
        }
        REQUIRE(graph_view->get_child_count() == initial_child_count + default_task_names.size());
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Create subtree task")
    {
        graph_view->create_subtree_task_node("subtree");
        BTGraphNodeSubtree* node = godot::Object::cast_to<BTGraphNodeSubtree>(get_graph_node(0));

        REQUIRE_NE(node, nullptr);
        CHECK_EQ(graph_view->get_graph_node("subtree"), node);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Sort task names by Y position")
    {
        this->create_default_graph();
        graph_view->set_node_position("task211", godot::Vector2(0, 100));
        graph_view->set_node_position("task212", godot::Vector2(0, 50));
        graph_view->set_node_position("task213", godot::Vector2(0, 150));
        graph_view->set_node_position("task214", godot::Vector2(0, 0));

        godot::Vector<StringName> unsorted_tasks = {"task211", "task212", "task213", "task214"};
        godot::Vector<StringName> sorted_tasks = graph_view->sorted_task_names_by_y(unsorted_tasks);

        REQUIRE_EQ(sorted_tasks.size(), 4);
        CHECK_EQ(sorted_tasks[0], godot::StringName("task214"));
        CHECK_EQ(sorted_tasks[1], godot::StringName("task212"));  
        CHECK_EQ(sorted_tasks[2], godot::StringName("task211"));
        CHECK_EQ(sorted_tasks[3], godot::StringName("task213"));  
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Find insert index by Y position")
    {
        this->create_default_graph();
        graph_view->set_node_position("task211", godot::Vector2(0, 100));
        graph_view->set_node_position("task212", godot::Vector2(0, 50));
        graph_view->set_node_position("task213", godot::Vector2(0, 150));
        graph_view->set_node_position("task214", godot::Vector2(0, 0));

        godot::Vector<StringName> task_list = {"task211", "task212", "task213", "task214"};

        graph_view->create_task_node("task_new");
        graph_view->set_node_position("task_new", godot::Vector2(0, 125));

        int index = graph_view->find_insert_index_by_y("task_new", task_list);
        CHECK_EQ(index, 3);

        graph_view->set_node_position("task_new", godot::Vector2(0, 75));

        index = graph_view->find_insert_index_by_y("task_new", task_list);
        CHECK_EQ(index, 2);

        graph_view->set_node_position("task_new", godot::Vector2(0, 100));

        index = graph_view->find_insert_index_by_y("task_new", task_list);
        CHECK((index == 3 || index == 2));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Deleting node removes it's connections")
    {
        this->create_default_graph();
        graph_view->create_task_node("task22");
        graph_view->connect_task_nodes("task2", "task22");

        CHECK_EQ(graph_view->get_connection_list().size(), 12);

        graph_view->delete_task_node("task2");

        CHECK_EQ(graph_view->get_connection_list().size(), 9);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Saves and removes connections")
    {
        create_default_graph();
        graph_view->clear_and_save_graph("somename");
        CHECK_EQ(graph_view->get_connection_list().size(), 0);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Loads connections between nodes")
    {
        create_default_graph();
        graph_view->clear_and_save_graph("somename");

        graph_view->load_graph("somename");
        CHECK_EQ(graph_view->get_connection_list().size(), 11);
        CHECK(graph_view->is_node_connected(root->get_name(), 0, task1->get_name(), 0));
        CHECK(graph_view->is_node_connected(task1->get_name(), 0, task11->get_name(), 0));
        CHECK(graph_view->is_node_connected(task2->get_name(), 0, task21->get_name(), 0));
        CHECK(graph_view->is_node_connected(task21->get_name(), 0, task211->get_name(), 0));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Can save again after one reload")
    {
        create_default_graph();
        graph_view->clear_and_save_graph("somename");
        graph_view->load_graph("somename");
        graph_view->clear_and_save_graph("somename");
        CHECK_EQ(graph_view->get_connection_list().size(), 0);
        CHECK_EQ(graph_view->get_child_count(), initial_child_count);
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Set root colors the root")
    {
        create_default_graph();
        graph_view->set_root_task_name("root");
        CHECK_EQ(graph_view->get_root_task_name(), godot::StringName("root"));
        CHECK_EQ(root->get_self_modulate(), godot::Color::named("BLUE"));

        graph_view->set_root_task_name("task1");
        CHECK_EQ(graph_view->get_root_task_name(), godot::StringName("task1"));
        CHECK_EQ(task1->get_self_modulate(), godot::Color::named("BLUE"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Removes color from old root")
    {
        create_default_graph();
        graph_view->set_root_task_name("root");
        graph_view->set_root_task_name("task1");
        CHECK_EQ(root->get_self_modulate(), godot::Color::named("WHITE"));
        graph_view->set_root_task_name("task2");
        CHECK_EQ(task1->get_self_modulate(), godot::Color::named("WHITE"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Get selected node task names")
    {
        create_default_graph();

        godot::Vector<godot::StringName> selected_names = graph_view->get_selected_node_task_names();
        REQUIRE(selected_names.size() == 0);

        graph_view->set_task_node_selected("task1", true);
        graph_view->set_task_node_selected("task21", true);
        graph_view->set_task_node_selected("task213", true);

        selected_names = graph_view->get_selected_node_task_names();

        REQUIRE(selected_names.size() == 3);
        CHECK(selected_names.has("task1"));
        CHECK(selected_names.has("task21"));
        CHECK(selected_names.has("task213"));
        CHECK(task1->is_selected());
        CHECK(task21->is_selected());
        CHECK(task213->is_selected());
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Deselect all nodes")
    {
        create_default_graph();
        graph_view->set_task_node_selected("task1", true);
        graph_view->set_task_node_selected("task21", true);
        graph_view->set_task_node_selected("task213", true);

        graph_view->deselect_all_nodes();

        godot::Vector<godot::StringName> selected_names = graph_view->get_selected_node_task_names();
        REQUIRE(selected_names.size() == 0);
        CHECK_FALSE(task1->is_selected());
        CHECK_FALSE(task21->is_selected());
        CHECK_FALSE(task213->is_selected());
    }
}

TEST_SUITE("[editor]" "[errors]" "BTGraphView")
{
    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to create task node")
    {
        SUBCASE("Non-existent nodes")
        {
            CHECK_GODOT_ERROR(graph_view->create_task_node("", "TaskClass"));
            CHECK_GODOT_ERROR(graph_view->create_task_node("task_1", ""));
        }

        SUBCASE("Fail to Create Duplicate Task Nodes")
        {
            graph_view->create_task_node("task_1");
            CHECK_GODOT_ERROR(graph_view->create_task_node("task_1"));
        }
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to create subtree node")
    {
        SUBCASE("Non-existent nodes")
        {
            CHECK_GODOT_ERROR(graph_view->create_subtree_task_node(""));
        }

        SUBCASE("Fail to Create Duplicate Subtree/Task Nodes")
        {
            graph_view->create_task_node("task_1");
            CHECK_GODOT_ERROR(graph_view->create_subtree_task_node("task_1"));
        }
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to set title of non-existent task")
    {
        CHECK_GODOT_ERROR(graph_view->set_task_node_title("non_existent", "Title"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to delete task node")
    {
        SUBCASE("Non-existent task node")
        {
            CHECK_GODOT_ERROR(graph_view->delete_task_node("non_existent"));
        }

        SUBCASE("Delete task node again")
        {
            graph_view->create_task_node("task_1");
            graph_view->delete_task_node("task_1");
            CHECK_GODOT_ERROR(graph_view->delete_task_node("task_1"));
        }
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

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to disconnect")
    {
        CHECK_GODOT_ERROR(graph_view->disconnect_task_nodes("non_existent", "task_1"));
        CHECK_GODOT_ERROR(graph_view->disconnect_task_nodes("task_1", "non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to set position to non-existent task nodes")
    {
        CHECK_GODOT_ERROR(graph_view->set_node_position("non_existent", godot::Vector2(0, 0)));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to get non-existent task nodes")
    {
        graph_view->create_task_node("task_1");
        CHECK_GODOT_ERROR(graph_view->get_graph_node("non_existent"));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to get tree map")
    {
        create_default_graph();

        SUBCASE("Non-existent parent")
        {
            parent_to_children_names["non_existent"] = {"root"};
            CHECK_GODOT_ERROR(graph_view->get_node_tree_map(parent_to_children_names));
        }

        SUBCASE("Non-existent child")
        {
            parent_to_children_names["task213"] = {"non_existent"};
            CHECK_GODOT_ERROR(graph_view->get_node_tree_map(parent_to_children_names));
        }
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to arrange nodes")
    {
        create_default_graph();

        SUBCASE("Non-existent parent")
        {
            parent_to_children_names["non_existent"] = {"root"};
            CHECK_GODOT_ERROR(graph_view->arrange_nodes("root", parent_to_children_names));
        }

        SUBCASE("Non-existent child")
        {
            parent_to_children_names["task213"] = {"non_existent"};
            CHECK_GODOT_ERROR(graph_view->arrange_nodes("root", parent_to_children_names));
        }

        SUBCASE("Non-existent root")
        {
            CHECK_GODOT_ERROR(graph_view->arrange_nodes("non_existent", parent_to_children_names));
        }
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to sort task names with non-existent task")
    {
        godot::Vector<StringName> tasks;
        graph_view->create_task_node("existing_task");
        tasks.push_back("existing_task");
        tasks.push_back("non_existent");

        CHECK_GODOT_ERROR(graph_view->sorted_task_names_by_y(tasks));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to find insert index with non-existent task")
    {
        godot::Vector<StringName> tasks;
        tasks.push_back("existent_task");

        CHECK_GODOT_ERROR(graph_view->find_insert_index_by_y("non_existent", tasks));
    }

    TEST_CASE_FIXTURE(BTGraphViewFixture, "Fail to get task name of non-existent graph node")
    {
        CHECK_GODOT_ERROR(graph_view->get_task_name("non_existent_graph_node"));
    }
}