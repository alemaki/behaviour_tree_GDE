#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "behaviour_tree/ui/bt_editor_plugin.hpp"
#include "behaviour_tree/ui/bt_graph_editor.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"
#include "tests/test_utils/signal_watcher.hpp"


/* Currently not running editor tests, since the engine doesn't support it very well. 
    TODO: Figure out a way to test the editor plugin. */



TEST_SUITE("[editor]" "[deprecated]" "BTGraphEditor")
{
    TEST_CASE("Graph editor tests")
    {
        /* Careful with testing undo redo methods. Make sure you return everything to original condition in the undo redo. */
        BTEditorPlugin* editor_plugin = BTEditorPlugin::get_singleton();
        REQUIRE(editor_plugin != nullptr);

        godot::EditorUndoRedoManager* undo_redo_manager = editor_plugin->get_undo_redo();
        REQUIRE(undo_redo_manager != nullptr);

        BTGraphEditor* editor = memnew(BTGraphEditor);
        editor->set_editor_plugin(editor_plugin); /* Need it for the undo redo manager */

        godot::GraphEdit* graph_edit = editor->get_graph_edit();

        REQUIRE(editor->get_graph_edit() != nullptr);

        BehaviourTree* tree = memnew(BehaviourTree);
        editor->set_behaviour_tree(tree);

        REQUIRE(editor->get_behaviour_tree() == tree);
        REQUIRE(graph_edit->get_child_count() == 4);

        SUBCASE("Test node creation")
        {
            editor->_add_new_node_button_pressed();

            REQUIRE(graph_edit->get_child_count() == 5);
            BTGraphNode* node = godot::Object::cast_to<BTGraphNode>(graph_edit->get_child(4));

            REQUIRE(node != nullptr);
            CHECK(node->get_task() != nullptr);
            CHECK(node->get_parent() == graph_edit);
            CHECK_MESSAGE(node->get_name() == godot::StringName("1"), "Default node name when created in empty graph should be 1.");

            SUBCASE("Test undo")
            {
                godot::UndoRedo* undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(tree));
                REQUIRE(undo_redo != nullptr);
                REQUIRE(undo_redo->has_undo());
                undo_redo->undo();
                CHECK(graph_edit->get_child_count() == 4);
                REQUIRE(node != nullptr);
                CHECK(node->get_parent() == nullptr);
            }
        }

        godot::UndoRedo* undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(tree));
        memdelete(editor);
        //memdelete(tree);
    }
/*
    TEST_CASE("Test Node Insertion and Erasure")
    {
        BTGraphEditor* editor = memnew(BTGraphEditor);
        BTGraphNode* node = editor->new_graph_node();

        editor->insert_node(node);
        REQUIRE(editor->get_graph_nodes().size() == 1);

        editor->erase_node(node);
        REQUIRE(editor->get_graph_nodes().size() == 0);

        memdelete(node);
        memdelete(editor);
    }

    TEST_CASE("Test Node Connection")
    {
        BTGraphEditor* editor = memnew(BTGraphEditor);
        BTGraphNode* node1 = editor->new_graph_node();
        BTGraphNode* node2 = editor->new_graph_node();

        editor->insert_node(node1);
        editor->insert_node(node2);
        editor->graph_edit->connect_node(node1->get_name(), 0, node2->get_name(), 0);

        REQUIRE(editor->graph_edit->is_connected(node1->get_name(), 0, node2->get_name(), 0));

        memdelete(node1);
        memdelete(node2);
        memdelete(editor);
    }

    TEST_CASE("Test Node Deletion and Undo/Redo")
    {
        BTGraphEditor* editor = memnew(BTGraphEditor);
        BTGraphNode* node = editor->new_graph_node();
        editor->insert_node(node);

        editor->delete_nodes({node});
        REQUIRE(editor->get_graph_nodes().size() == 0);

        godot::EditorUndoRedoManager* undo_redo_manager = editor->editor_plugin->get_undo_redo();
        undo_redo_manager->redo();
        REQUIRE(editor->get_graph_nodes().size() == 1);

        memdelete(node);
        memdelete(editor);
    }

    TEST_CASE("Test Root Node Setting")
    {
        BTGraphEditor* editor = memnew(BTGraphEditor);
        BTGraphNode* node = editor->new_graph_node();
        editor->insert_node(node);

        editor->set_root_node(node);
        REQUIRE(editor->task_to_node[editor->behaviour_tree->get_root_task()] == node);

        memdelete(node);
        memdelete(editor);
    }

    TEST_CASE("Test Node Sorting by Y Position")
    {
        BTGraphEditor* editor = memnew(BTGraphEditor);
        BTGraphNode* node1 = editor->new_graph_node();
        BTGraphNode* node2 = editor->new_graph_node();

        node1->set_position_offset(godot::Vector2(0, 10));
        node2->set_position_offset(godot::Vector2(0, 5));

        editor->insert_node(node1);
        editor->insert_node(node2);

        godot::Array sorted_nodes = editor->get_sorted_by_y_children_of_parent(node1);

        REQUIRE(sorted_nodes.size() == 2);
        REQUIRE(sorted_nodes[0] == node2);
        REQUIRE(sorted_nodes[1] == node1);

        memdelete(node1);
        memdelete(node2);
        memdelete(editor);
    }*/
}