#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "behaviour_tree/ui/bt_editor_plugin.hpp"
#include "behaviour_tree/ui/bt_graph_editor.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"
#include "tests/test_utils/signal_watcher.hpp"

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
};


TEST_SUITE("[editor]" "BTGraphEditor")
{
    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Graph editor basic")
    {
        REQUIRE_EQ(editor->get_behaviour_tree(), tree);
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test node creation")
    {
        editor->_add_new_node_button_pressed();

        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 1);
        BTGraphNode* node = get_graph_node(0);

        REQUIRE_NE(node, nullptr);
        CHECK_EQ(graph_view->get_graph_node("1"), node);
        CHECK_EQ(node->get_title(), "1");

        
        REQUIRE_EQ(tree->get_task_count(), 1);
        godot::Ref<BTTask> task = tree->get_task(1);
        REQUIRE(task.is_valid());
        CHECK_EQ(task->get_custom_name(), "1");
    }
}

TEST_SUITE("[editor]" "[undo_redo]" "BTGraphEditor")
{
    TEST_CASE_FIXTURE(BTGraphEditorFixture, "[undo_redo]" "[undo_redo] Test node creation")
    {
        editor->_add_new_node_button_pressed();

        godot::Ref<BTTask> task = tree->get_task(1);
        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();

        CHECK_EQ(graph_view->get_child_count(), initial_child_count);
        REQUIRE_EQ(tree->get_task_count(), 0);

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        CHECK_EQ(graph_view->get_child_count(), initial_child_count + 1);
        BTGraphNode* node = get_graph_node(0);
        REQUIRE_NE(node, nullptr);
        REQUIRE_EQ(node->get_title(), "1");
        REQUIRE_EQ(tree->get_task_count(), 1);
        REQUIRE(tree->has_task(task));

    }
}