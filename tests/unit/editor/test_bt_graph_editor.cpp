#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "behaviour_tree/ui/bt_editor_plugin.hpp"
#include "behaviour_tree/ui/bt_graph_editor.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"
#include "tests/test_utils/signal_watcher.hpp"
#include "tests/test_utils/test_utils.hpp"

struct BTGraphEditorFixture
{
    /* Careful with testing undo redo methods. Make sure you return everything to original condition in the undo redo. */
    BTEditorPlugin* editor_plugin = BTEditorPlugin::get_singleton();
    godot::EditorUndoRedoManager* undo_redo_manager = nullptr;
    BTGraphEditor* editor = nullptr;
    BehaviourTree* tree = nullptr;
    BTGraphView* graph_view = nullptr;
    int initial_child_count = 0;

    BTGraphEditorFixture()
    {
        REQUIRE_NE(editor_plugin, nullptr);
        this->undo_redo_manager = editor_plugin->get_undo_redo();
        REQUIRE_NE(undo_redo_manager, nullptr);
        this->editor = memnew(BTGraphEditor);
        this->editor->set_editor_plugin(editor_plugin); /* Need it for the undo redo manager */
        this->tree = memnew(BehaviourTree);
        this->editor->set_behaviour_tree(tree);
        this->graph_view = this->editor->get_graph_view();
        REQUIRE_NE(this->graph_view, nullptr);
        this->initial_child_count = this->graph_view->get_child_count();
    }

    BTGraphNode* get_graph_node(int index)
    {
       return godot::Object::cast_to<BTGraphNode>(this->graph_view->get_child(initial_child_count + index));
    }

    godot::UndoRedo* get_undo(godot::Object* object)
    {
        godot::UndoRedo* undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(object));
        REQUIRE_NE(undo_redo, nullptr);
        REQUIRE(undo_redo->has_undo());
        return undo_redo;

    }

    godot::UndoRedo* get_redo(godot::Object* object)
    {
        godot::UndoRedo* undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(object));
        REQUIRE_NE(undo_redo, nullptr);
        REQUIRE(undo_redo->has_redo());
        return undo_redo;
    }

    ~BTGraphEditorFixture()
    {
        godot::UndoRedo* undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(this->tree));
        undo_redo->clear_history(false);

        undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(this->graph_view));
        undo_redo->clear_history(false);

        undo_redo = undo_redo_manager->get_history_undo_redo(godot::EditorUndoRedoManager::SpecialHistory::GLOBAL_HISTORY);
        undo_redo->clear_history(false);

        this->tree->queue_free();
        this->editor->queue_free();
    }


    void test_node_creation()
    {
        editor->_add_new_node_button_pressed();

        REQUIRE_EQ(tree->get_task_count(), 1);
        godot::Ref<BTTask> task = tree->get_task(1);
        REQUIRE(task.is_valid());
        CHECK_EQ(task->get_custom_name(), "1");

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 1);
        BTGraphNode* node = get_graph_node(0);
        godot::StringName task_name = task->get_name();

        REQUIRE_NE(node, nullptr);
        CHECK_EQ(graph_view->get_graph_node(task_name), node);
        CHECK_EQ(node->get_title(), "1");
    }

    void test_subtree_creation()
    {
        editor->_add_new_subtree_node_button_pressed();


        REQUIRE_EQ(tree->get_task_count(), 1);
        godot::Ref<BTSubtree> task = tree->get_task(1);
        REQUIRE(task.is_valid());
        CHECK_EQ(task->get_custom_name(), "1");
        CHECK_EQ(task->get_file_path(), "");

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 1);
        BTGraphNodeSubtree* node = godot::Object::cast_to<BTGraphNodeSubtree>(get_graph_node(0));
        godot::StringName task_name = task->get_name();

        REQUIRE_NE(node, nullptr);
        CHECK_EQ(graph_view->get_graph_node(task_name), node);
        CHECK_EQ(node->get_title(), "");
        CHECK_EQ(node->get_file_path(), "");
    }

    void test_connection_creation()
    {
        editor->_add_new_node_button_pressed();
        editor->_add_new_subtree_node_button_pressed();

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 2);
        BTGraphNode* parent = get_graph_node(0);
        BTGraphNode* child = get_graph_node(1);

        REQUIRE_NE(parent, nullptr);
        REQUIRE_NE(child, nullptr);

        editor->connection_request(parent->get_name(), 0, child->get_name(), 0);

        REQUIRE_EQ(tree->get_task_count(), 2);

        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> child_task = tree->get_task(2);

        REQUIRE(parent_task.is_valid());
        REQUIRE(child_task.is_valid());
        CHECK_EQ(parent_task->get_custom_name(), "1");
        CHECK_EQ(child_task->get_custom_name(), "2");
        REQUIRE_EQ(parent_task->get_child_count(), 1);
        CHECK(parent_task->has_child(child_task));

        REQUIRE_EQ(graph_view->get_connection_list().size(), 1);
        REQUIRE(graph_view->is_node_connected(parent->get_name(), 0, child->get_name(), 0));
    }

    void test_connection_deletion()
    {
        editor->_add_new_node_button_pressed();
        editor->_add_new_node_button_pressed();

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 2);
        BTGraphNode* parent = get_graph_node(0);
        BTGraphNode* child = get_graph_node(1);

        REQUIRE_NE(parent, nullptr);
        REQUIRE_NE(child, nullptr);

        editor->connection_request(parent->get_name(), 0, child->get_name(), 0);
        editor->disconnection_request(parent->get_name(), 0, child->get_name(), 0);

        REQUIRE_EQ(graph_view->get_connection_list().size(), 0);

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 2);

        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> child_task = tree->get_task(2);
        REQUIRE(parent_task.is_valid());
        REQUIRE(child_task.is_valid());
        REQUIRE_EQ(parent_task->get_child_count(), 0);
        REQUIRE_FALSE(parent_task->has_child(child_task));
    }

    void test_node_moving()
    {
        editor->_add_new_node_button_pressed();
        editor->_add_new_subtree_node_button_pressed();
        editor->_add_new_subtree_node_button_pressed();

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 3);
        BTGraphNode* parent = get_graph_node(0);
        BTGraphNode* child1 = get_graph_node(1);
        BTGraphNode* child2 = get_graph_node(2);

        REQUIRE_NE(parent, nullptr);
        REQUIRE_NE(child1, nullptr);
        REQUIRE_NE(child2, nullptr);

        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> child_task1 = tree->get_task(2);
        godot::Ref<BTTask> child_task2 = tree->get_task(3);
        REQUIRE(parent_task.is_valid());
        REQUIRE(child_task1.is_valid());
        REQUIRE(child_task2.is_valid());

        graph_view->set_node_position(child_task1->get_name(), godot::Vector2(0, 0));
        graph_view->set_node_position(child_task2->get_name(), godot::Vector2(0, 50));

        editor->connection_request(parent->get_name(), 0, child1->get_name(), 0);
        editor->connection_request(parent->get_name(), 0, child2->get_name(), 0);

        REQUIRE_EQ(graph_view->get_connection_list().size(), 2);

        REQUIRE_EQ(parent_task->get_child_count(), 2);
        REQUIRE_EQ(parent_task->get_child(0), child_task1);
        REQUIRE_EQ(parent_task->get_child(1), child_task2);

        graph_view->set_node_position(child_task2->get_name(), godot::Vector2(0, -50));
        graph_view->set_node_position(child_task1->get_name(), godot::Vector2(0, 50));

        editor->_node_dragged(godot::Vector2(0, 50), godot::Vector2(0, -50), child2);
        editor->_node_dragged(godot::Vector2(0, 0), godot::Vector2(0, 50), child1);

        editor->_move_nodes();

        CHECK_VECTORS_EQ(parent->get_position_offset(), godot::Vector2(0, 0));
        CHECK_VECTORS_EQ(child1->get_position_offset(), godot::Vector2(0, 50));
        CHECK_VECTORS_EQ(child2->get_position_offset(), godot::Vector2(0, -50));

        REQUIRE_EQ(parent_task->get_child_count(), 2);
        CHECK_EQ(parent_task->get_child(0), child_task2);
        CHECK_EQ(parent_task->get_child(1), child_task1);
    }
};


TEST_SUITE("[editor]" "[plugin]" "BTGraphEditor")
{
    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Graph editor basic")
    {
        REQUIRE_EQ(editor->get_behaviour_tree(), tree);
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test node creation")
    {
        test_node_creation();
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test subtree creation")
    {
        test_subtree_creation();
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test connection creation")
    {
        test_connection_creation();
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test connection deletion")
    {
        test_connection_deletion();
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test move nodes")
    {
        test_node_moving();
    }
}

TEST_SUITE("[editor]" "[plugin]" "[undo_redo]" "BTGraphEditor")
{
    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test node creation")
    {
        test_node_creation();

        godot::Ref<BTTask> task = tree->get_task(1);
        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();

        CHECK_EQ(graph_view->get_child_count(), initial_child_count);
        REQUIRE_EQ(tree->get_task_count(), 0);

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 1);
        REQUIRE_EQ(tree->get_task_count(), 1);
        BTGraphNode* node = get_graph_node(0);
        REQUIRE_NE(node, nullptr);
        CHECK_EQ(node->get_title(), "1");
        CHECK_EQ(tree->get_task_count(), 1);
        REQUIRE(tree->has_task(task));

    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test subtree creation")
    {
        test_subtree_creation();

        godot::Ref<BTTask> task = tree->get_task(1);
        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count);
        REQUIRE_EQ(tree->get_task_count(), 0);

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 1);
        REQUIRE_EQ(tree->get_task_count(), 1);
        BTGraphNodeSubtree* node = godot::Object::cast_to<BTGraphNodeSubtree>(get_graph_node(0));
        REQUIRE_NE(node, nullptr);
        CHECK_EQ(node->get_title(), "");
        CHECK_EQ(tree->get_task_count(), 1);
        CHECK(tree->has_task(task));

    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test connection creation")
    {
        test_connection_creation();

        BTGraphNode* parent = get_graph_node(0);
        BTGraphNode* child = get_graph_node(1);

        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> child_task = tree->get_task(2);

        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();

        REQUIRE_EQ(graph_view->get_connection_list().size(), 0);
        REQUIRE_EQ(parent_task->get_child_count(), 0);

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        REQUIRE_EQ(parent_task->get_child_count(), 1);
        REQUIRE_EQ(parent_task->get_child(0), child_task);
        REQUIRE_EQ(graph_view->get_connection_list().size(), 1);
        REQUIRE(graph_view->is_node_connected(parent->get_name(), 0, child->get_name(), 0));
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test connection deletion")
    {
        test_connection_deletion();

        BTGraphNode* parent = get_graph_node(0);
        BTGraphNode* child = get_graph_node(1);

        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> child_task = tree->get_task(2);

        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();


        REQUIRE_EQ(parent_task->get_child_count(), 1);
        REQUIRE_EQ(parent_task->get_child(0), child_task);
        REQUIRE_EQ(graph_view->get_connection_list().size(), 1);
        REQUIRE(graph_view->is_node_connected(parent->get_name(), 0, child->get_name(), 0));

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        REQUIRE_EQ(graph_view->get_connection_list().size(), 0);
        REQUIRE_EQ(parent_task->get_child_count(), 0);
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test move nodes")
    {
        test_node_moving();

        BTGraphNode* parent = get_graph_node(0);
        BTGraphNode* child1 = get_graph_node(1);
        BTGraphNode* child2 = get_graph_node(2);

        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> child_task1 = tree->get_task(2);
        godot::Ref<BTTask> child_task2 = tree->get_task(3);

        godot::Vector2 parent_pos = parent->get_position_offset();
        godot::Vector2 child1_pos = child1->get_position_offset();
        godot::Vector2 child2_pos = child2->get_position_offset();

        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();

        REQUIRE_EQ(parent->get_position_offset(),  godot::Vector2(0, 0));
        REQUIRE_EQ(child1->get_position_offset(),  godot::Vector2(0, 0));
        REQUIRE_EQ(child2->get_position_offset(),  godot::Vector2(0, 50));

        REQUIRE_EQ(parent_task->get_child_count(), 2);
        REQUIRE_EQ(parent_task->get_child(0), child_task1);
        REQUIRE_EQ(parent_task->get_child(1), child_task2);

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        CHECK_VECTORS_EQ(parent->get_position_offset(), godot::Vector2(0, 0));
        CHECK_VECTORS_EQ(child1->get_position_offset(), godot::Vector2(0, 50));
        CHECK_VECTORS_EQ(child2->get_position_offset(), godot::Vector2(0, -50));

        REQUIRE_EQ(parent_task->get_child_count(), 2);
        CHECK_EQ(parent_task->get_child(0), child_task2);
        CHECK_EQ(parent_task->get_child(1), child_task1);

    }
}