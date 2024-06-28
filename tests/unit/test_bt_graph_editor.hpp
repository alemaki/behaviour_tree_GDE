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
        REQUIRE_NE(editor_plugin, nullptr);

        godot::EditorUndoRedoManager* undo_redo_manager = editor_plugin->get_undo_redo();
        REQUIRE_NE(undo_redo_manager, nullptr);

        BTGraphEditor* editor = memnew(BTGraphEditor);
        editor->set_editor_plugin(editor_plugin); /* Need it for the undo redo manager */

        godot::GraphEdit* graph_edit = editor->get_graph_edit();

        REQUIRE_NE(editor->get_graph_edit(), nullptr);

        BehaviourTree* tree = memnew(BehaviourTree);
        editor->set_behaviour_tree(tree);

        REQUIRE_EQ(editor->get_behaviour_tree(), tree);
        REQUIRE_EQ(graph_edit->get_child_count(), 4);

        SUBCASE("Test node creation")
        {
            editor->_add_new_node_button_pressed();

            REQUIRE_EQ(graph_edit->get_child_count(), 5);
            BTGraphNode* node = godot::Object::cast_to<BTGraphNode>(graph_edit->get_child(4));

            REQUIRE_NE(node, nullptr);
            CHECK_NE(node->get_task(), nullptr);
            CHECK_EQ(node->get_parent(), graph_edit);
            CHECK_MESSAGE(node->get_name() == godot::StringName("1"), "Default node name when created in empty graph should be 1.");

            SUBCASE("Test undo")
            {
                godot::UndoRedo* undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(tree));
                REQUIRE_NE(undo_redo, nullptr);
                REQUIRE(undo_redo->has_undo());
                undo_redo->undo();
                CHECK_EQ(graph_edit->get_child_count(), 4);
                REQUIRE_NE(node, nullptr);
                CHECK_EQ(node->get_parent(), nullptr);
            }
        }

        godot::UndoRedo* undo_redo = undo_redo_manager->get_history_undo_redo(undo_redo_manager->get_object_history_id(tree));
        memdelete(editor);
        memdelete(tree);
    }
}