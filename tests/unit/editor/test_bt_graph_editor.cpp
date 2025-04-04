#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#include "tests/test_utils/test_macros.hpp"

#include "behaviour_tree/ui/bt_editor_plugin.hpp"
#include "behaviour_tree/ui/bt_graph_editor.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"

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

        editor->_node_dragged(godot::Vector2(0, 50), godot::Vector2(0, -50), child_task2->get_name());
        editor->_node_dragged(godot::Vector2(0, 0), godot::Vector2(0, 50), child_task1->get_name());

        editor->_move_nodes();

        CHECK_VECTORS_EQ(parent->get_position_offset(), godot::Vector2(0, 0));
        CHECK_VECTORS_EQ(child1->get_position_offset(), godot::Vector2(0, 50));
        CHECK_VECTORS_EQ(child2->get_position_offset(), godot::Vector2(0, -50));

        REQUIRE_EQ(parent_task->get_child_count(), 2);
        CHECK_EQ(parent_task->get_child(0), child_task2);
        CHECK_EQ(parent_task->get_child(1), child_task1);
    }

    void assert_node_before_deletion()
    {
        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> node1_task = tree->get_task(2);
        godot::Ref<BTTask> node11_task = tree->get_task(3);
        godot::Ref<BTTask> node12_task = tree->get_task(4);

        BTGraphNode* parent = graph_view->get_graph_node(parent_task->get_name());
        BTGraphNode* node1 = graph_view->get_graph_node(node1_task->get_name());
        BTGraphNode* node11 = graph_view->get_graph_node(node11_task->get_name());
        BTGraphNode* node12 = graph_view->get_graph_node(node12_task->get_name());

        REQUIRE_NE(parent, nullptr);
        REQUIRE_NE(node1, nullptr);
        REQUIRE_NE(node11, nullptr);
        REQUIRE_NE(node12, nullptr);
        CHECK_VECTORS_EQ(graph_view->get_graph_node(node1_task->get_name())->get_position_offset(), godot::Vector2(0, 1));
        REQUIRE_EQ(graph_view->get_connection_list().size(), 3);
        REQUIRE_EQ(parent_task->get_child_count(), 1);
        CHECK_EQ(parent_task->get_child(0), node1_task);
        CHECK_EQ(node1_task->get_child(0), node11_task);
        CHECK_EQ(node1_task->get_child(1), node12_task);
        CHECK(graph_view->is_node_connected(parent->get_name(), 0, node1->get_name(), 0));
        CHECK(graph_view->is_node_connected(node1->get_name(), 0, node11->get_name(), 0));
        CHECK(graph_view->is_node_connected(node1->get_name(), 0, node12->get_name(), 0));
    }

    void assert_node_deleted(godot::Ref<BTTask> deleted_task)
    {
        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> node11_task = tree->get_task(3);
        godot::Ref<BTTask> node12_task = tree->get_task(4);

        CHECK_EQ(tree->get_task_count(), 3);
        CHECK_FALSE(tree->has_task(deleted_task));
        CHECK_EQ(graph_view->get_connection_list().size(), 0);
        CHECK_EQ(graph_view->get_child_count(), initial_child_count + 3);
        CHECK_EQ(parent_task->get_child_count(), 0);
        CHECK_EQ(deleted_task->get_child_count(), 0);
        CHECK(node11_task->get_parent().is_null());
        CHECK(node12_task->get_parent().is_null());
    }

    void test_node_deletion()
    {
        editor->_add_new_node_button_pressed();
        editor->_add_new_node_button_pressed();
        editor->_add_new_node_button_pressed();
        editor->_add_new_node_button_pressed();
        REQUIRE_EQ(tree->get_task_count(), 4);
        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 4);

        
        godot::Ref<BTTask> parent_task = tree->get_task(1);
        godot::Ref<BTTask> node1_task = tree->get_task(2);
        godot::Ref<BTTask> node11_task = tree->get_task(3);
        godot::Ref<BTTask> node12_task = tree->get_task(4);

        REQUIRE_NE(graph_view->get_graph_node(parent_task->get_name()), nullptr);
        REQUIRE_NE(graph_view->get_graph_node(node1_task->get_name()), nullptr);
        REQUIRE_NE(graph_view->get_graph_node(node11_task->get_name()), nullptr);
        REQUIRE_NE(graph_view->get_graph_node(node12_task->get_name()), nullptr);

        graph_view->set_node_position(node1_task->get_name(), godot::Vector2(0, 1));

        editor->connection_request(graph_view->get_graph_node(parent_task->get_name())->get_name(), 0, graph_view->get_graph_node(node1_task->get_name())->get_name(), 0);
        editor->connection_request(graph_view->get_graph_node(node1_task->get_name())->get_name(), 0, graph_view->get_graph_node(node11_task->get_name())->get_name(), 0);
        editor->connection_request(graph_view->get_graph_node(node1_task->get_name())->get_name(), 0, graph_view->get_graph_node(node12_task->get_name())->get_name(), 0);

        assert_node_before_deletion();

        godot::TypedArray<godot::StringName> nodes_to_delete;
        nodes_to_delete.push_back(graph_view->get_graph_node(node1_task->get_name())->get_name());
        editor->_delete_nodes_request(nodes_to_delete);

        assert_node_deleted(node1_task);
    }
};


struct BTGraphEditorReadyTreeFixture : public BTGraphEditorFixture
{
    godot::Ref<BTTask> root_task    = memnew(BTTask);
    godot::Ref<BTTask> child1_task  = memnew(BTTask);
    godot::Ref<BTTask> child2_task  = memnew(BTTask);
    godot::Ref<BTTask> child11_task = memnew(BTTask);
    godot::Ref<BTTask> child12_task = memnew(BTTask);
    godot::Ref<BTTask> child21_task = memnew(BTTask);
    godot::Ref<BTTask> child22_task = memnew(BTTask);
    BTGraphNode* root    = nullptr;
    BTGraphNode* child1  = nullptr;
    BTGraphNode* child2  = nullptr;
    BTGraphNode* child11 = nullptr;
    BTGraphNode* child12 = nullptr;
    BTGraphNode* child21 = nullptr;
    BTGraphNode* child22 = nullptr;

    BTGraphEditorReadyTreeFixture()
    {
        create_default_graph();
    }

    void create_default_graph()
    {
        BehaviourTree* old_tree = this->tree;
        this->tree = memnew(BehaviourTree);
        this->tree->add_task_by_ref(root_task);
        this->tree->add_task_by_ref(child1_task);
        this->tree->add_task_by_ref(child2_task);
        this->tree->add_task_by_ref(child11_task);
        this->tree->add_task_by_ref(child12_task);
        this->tree->add_task_by_ref(child21_task);
        this->tree->add_task_by_ref(child22_task);
        this->tree->connect_tasks(root_task,     child1_task);
        this->tree->connect_tasks(root_task,     child2_task, 1);
        this->tree->connect_tasks(child1_task,   child11_task);
        this->tree->connect_tasks(child1_task,   child12_task, 1);
        this->tree->connect_tasks(child2_task,   child21_task);
        this->tree->connect_tasks(child2_task,   child22_task, 1);
        this->editor->set_behaviour_tree(this->tree);
        this->root = graph_view->get_graph_node(root_task->get_name());
        this->child1 = graph_view->get_graph_node(child1_task->get_name());
        this->child2 = graph_view->get_graph_node(child2_task->get_name());
        this->child11 = graph_view->get_graph_node(child11_task->get_name());
        this->child12 = graph_view->get_graph_node(child12_task->get_name());
        this->child21 = graph_view->get_graph_node(child21_task->get_name());
        this->child22 = graph_view->get_graph_node(child22_task->get_name());
        old_tree->queue_free(); /* Since it was created in the previous constructor just clear them up */
    }

    void test_task_name_change()
    {
        editor->_on_node_double_clicked(root_task->get_name());
        editor->_on_rename_edit_text_submitted("root3K");
        CHECK_EQ(root_task->get_custom_name(), "root3K");
        CHECK_EQ(root->get_title(), "root3K");
    }

    void test_task_class_change()
    {
        editor->_change_task_type("BTCondition", root_task->get_name());

        godot::Ref<BTTask> new_task = tree->get_root_task();
        REQUIRE_FALSE(graph_view->has_task_name(root_task->get_name()));
        REQUIRE(graph_view->has_task_name(new_task->get_name()));

        BTGraphNode* root = graph_view->get_graph_node(new_task->get_name());

        REQUIRE_NE(root, nullptr);
        CHECK_EQ(new_task->get_class(), "BTCondition");
        CHECK_EQ(root->get_task_class_name(), godot::StringName("BTCondition"));
        CHECK_EQ(graph_view->get_task_name(root->get_name()), new_task->get_name());
    }

    void assert_task_changed(const godot::StringName& class_name, godot::Ref<BTTask> new_task, godot::Ref<BTTask> old_task)
    {
        REQUIRE(tree->has_task(new_task));
        REQUIRE_FALSE(tree->has_task(old_task));

        REQUIRE_FALSE(graph_view->has_task_name(old_task->get_name()));
        REQUIRE(graph_view->has_task_name(new_task->get_name()));

        BTGraphNode* root = graph_view->get_graph_node(new_task->get_name());

        REQUIRE_EQ(old_task->get_child_count(), 0);
        REQUIRE_NE(root, nullptr);
        REQUIRE_EQ(new_task->get_child_count(), 2);
        CHECK_EQ(new_task->get_child(0), child11_task);
        CHECK_EQ(new_task->get_child(1), child12_task);
        CHECK_EQ(new_task->get_parent(), root_task);
        CHECK_EQ(new_task->get_class(), class_name);
        CHECK_EQ(child1->get_task_class_name(), godot::StringName(class_name));
    }

    void test_task_class_change_keeps_connections()
    {
        editor->_change_task_type("BTCondition", child1_task->get_name());

        godot::Ref<BTTask> new_task = tree->get_root_task()->get_child(0);
        assert_task_changed("BTCondition", new_task, child1_task);
    }

    void test_copy_nodes_request()
    {
        graph_view->set_task_node_selected(child1_task->get_name(), true);
        graph_view->set_task_node_selected(child2_task->get_name(), true);

        editor->copy_nodes_request();
        const BTGraphEditor::CopyInfo& copy_info = editor->get_copy_info();
        CHECK_EQ(copy_info.copied_tasks.size(), 2);
        CHECK(copy_info.copied_tasks.has(child1_task));
        CHECK(copy_info.copied_tasks.has(child2_task));
        CHECK_EQ(copy_info.copied_positions.size(), 2);
        CHECK(copy_info.copied_positions.has(child1_task));
        CHECK(copy_info.copied_positions.has(child2_task));
        CHECK_EQ(copy_info.copied_connections.size(), 0);
    }

    void test_copy_nodes_copies_connections()
    {
        graph_view->set_task_node_selected(child1_task->get_name(), true);
        graph_view->set_task_node_selected(child2_task->get_name(), true);
        graph_view->set_task_node_selected(child11_task->get_name(), true);
        graph_view->set_task_node_selected(child12_task->get_name(), true);
        graph_view->set_task_node_selected(child21_task->get_name(), true);
        graph_view->set_task_node_selected(child22_task->get_name(), true);

        editor->copy_nodes_request();
        const BTGraphEditor::CopyInfo& copy_info = editor->get_copy_info();

        CHECK_EQ(copy_info.copied_tasks.size(), 6);
        CHECK_EQ(copy_info.copied_positions.size(), 6);
        REQUIRE_EQ(copy_info.copied_connections.size(), 2);
        REQUIRE(copy_info.copied_connections.has(child1_task));
        REQUIRE(copy_info.copied_connections.has(child2_task));
        CHECK_EQ(copy_info.copied_connections[child1_task][0], child11_task);
        CHECK_EQ(copy_info.copied_connections[child1_task][1], child12_task);
        CHECK_EQ(copy_info.copied_connections[child2_task][0], child21_task);
        CHECK_EQ(copy_info.copied_connections[child2_task][1], child22_task);
    }

    void test_paste_nodes_request()
    {
        graph_view->set_task_node_selected(child1_task->get_name(), true);
        graph_view->set_task_node_selected(child2_task->get_name(), true);
        editor->copy_nodes_request();
        editor->paste_nodes_request();
        
        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 7 + 2);
        REQUIRE_EQ(tree->get_task_count(), 7 + 2);
        godot::Ref<BTTask> new_task1 = this->tree->get_task(8);
        godot::Ref<BTTask> new_task2 = this->tree->get_task(9);
        REQUIRE(new_task1.is_valid());
        REQUIRE(new_task2.is_valid());
        REQUIRE_NE(new_task1->get_name(), child1_task->get_name());
        REQUIRE_NE(new_task2->get_name(), child2_task->get_name());
        CHECK_EQ(new_task1->get_custom_name(), child1_task->get_custom_name());
        CHECK_EQ(new_task2->get_custom_name(), child2_task->get_custom_name());
        CHECK_EQ(new_task1->get_class(), child1_task->get_class());
        CHECK_EQ(new_task2->get_class(), child2_task->get_class());
        REQUIRE(graph_view->has_task_name(new_task1->get_name()));
        REQUIRE(graph_view->has_task_name(new_task2->get_name()));
        CHECK_EQ(graph_view->get_graph_node(new_task1->get_name())->get_title(), child1->get_title());
        CHECK_EQ(graph_view->get_graph_node(new_task2->get_name())->get_title(), child2->get_title());
        CHECK_EQ(graph_view->get_graph_node(new_task1->get_name())->get_task_class_name(), child1->get_task_class_name());
        CHECK_EQ(graph_view->get_graph_node(new_task2->get_name())->get_task_class_name(), child2->get_task_class_name());
    }

    void test_paste_nodes_with_connections_request()
    {
        graph_view->set_task_node_selected(root_task->get_name(), true);
        graph_view->set_task_node_selected(child1_task->get_name(), true);
        graph_view->set_task_node_selected(child2_task->get_name(), true);
        graph_view->set_task_node_selected(child11_task->get_name(), true);
        graph_view->set_task_node_selected(child12_task->get_name(), true);
        graph_view->set_task_node_selected(child21_task->get_name(), true);
        graph_view->set_task_node_selected(child22_task->get_name(), true);
        editor->copy_nodes_request();
        editor->paste_nodes_request();
        
        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + 7 + 7);
        REQUIRE_EQ(tree->get_task_count(), 7 + 7);
        REQUIRE_EQ(graph_view->get_connection_list().size(), 6 + 6);
        godot::Ref<BTTask> new_root = this->tree->get_task(8);
        godot::Ref<BTTask> new_task1 = this->tree->get_task(9);
        godot::Ref<BTTask> new_task2 = this->tree->get_task(10);
        godot::Ref<BTTask> new_task11 = this->tree->get_task(11);
        godot::Ref<BTTask> new_task12 = this->tree->get_task(12);
        godot::Ref<BTTask> new_task21 = this->tree->get_task(13);
        godot::Ref<BTTask> new_task22 = this->tree->get_task(14);
        REQUIRE_EQ(new_root->get_child_count(), 2);
        REQUIRE_EQ(new_task1->get_child_count(), 2);
        REQUIRE_EQ(new_task2->get_child_count(), 2);
        REQUIRE_EQ(new_task22->get_child_count(), 0);
        REQUIRE_EQ(new_task11->get_child_count(), 0);
        CHECK_EQ(new_root->get_child(0), new_task1);
        CHECK_EQ(new_root->get_child(1), new_task2);
        CHECK_EQ(new_task1->get_child(0), new_task11);
        CHECK_EQ(new_task1->get_child(1), new_task12);
        CHECK_EQ(new_task2->get_child(0), new_task21);
        CHECK_EQ(new_task2->get_child(1), new_task22);

        CHECK(graph_view->is_node_connected(graph_view->get_graph_node(new_root->get_name())->get_name(), 0, graph_view->get_graph_node(new_task1->get_name())->get_name(), 0));
        CHECK(graph_view->is_node_connected(graph_view->get_graph_node(new_root->get_name())->get_name(), 0, graph_view->get_graph_node(new_task2->get_name())->get_name(), 0));
        CHECK(graph_view->is_node_connected(graph_view->get_graph_node(new_task1->get_name())->get_name(), 0, graph_view->get_graph_node(new_task11->get_name())->get_name(), 0));
        CHECK(graph_view->is_node_connected(graph_view->get_graph_node(new_task1->get_name())->get_name(), 0, graph_view->get_graph_node(new_task12->get_name())->get_name(), 0));
        CHECK(graph_view->is_node_connected(graph_view->get_graph_node(new_task2->get_name())->get_name(), 0, graph_view->get_graph_node(new_task21->get_name())->get_name(), 0));
        CHECK(graph_view->is_node_connected(graph_view->get_graph_node(new_task2->get_name())->get_name(), 0, graph_view->get_graph_node(new_task22->get_name())->get_name(), 0));
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

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test node deletion")
    {
        test_node_deletion();
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Default graph creation creates valid nodes and connections")
    {
        REQUIRE_EQ(graph_view->get_child_count(), initial_child_count + this->tree->get_task_count());
        godot::Array tasks = this->tree->get_tasks();
        for (int i = 0; i < tasks.size(); i++)
        {
            godot::Ref<BTTask> task = tasks[i];
            BTGraphNode* node = this->graph_view->get_graph_node(task->get_name());
            REQUIRE_NE(node, nullptr);
        }
     
        REQUIRE_EQ(graph_view->get_connection_list().size(), 6);
        CHECK(graph_view->is_node_connected(root->get_name(), 0, child1->get_name(), 0));
        CHECK(graph_view->is_node_connected(root->get_name(), 0, child2->get_name(), 0));
        CHECK(graph_view->is_node_connected(child1->get_name(), 0, child11->get_name(), 0));
        CHECK(graph_view->is_node_connected(child1->get_name(), 0, child12->get_name(), 0));
        CHECK(graph_view->is_node_connected(child2->get_name(), 0, child21->get_name(), 0));
        CHECK(graph_view->is_node_connected(child2->get_name(), 0, child22->get_name(), 0));
    }
    
    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Default graph arranges the nodes properly")
    {
        godot::Ref<BTGraphSortAlgorithm> graph_sort_algorithm = memnew(BTGraphSortAlgorithm);
        godot::HashMap<godot::StringName, godot::Vector<godot::StringName>> parent_to_children_names;
        parent_to_children_names[root_task->get_name()] = {child1_task->get_name(), child2_task->get_name()};
        parent_to_children_names[child1_task->get_name()] = {child11_task->get_name(), child12_task->get_name()};
        parent_to_children_names[child2_task->get_name()] = {child21_task->get_name(), child22_task->get_name()};
        godot::HashMap<BTGraphNode*, godot::Vector<BTGraphNode*>> parent_to_children = graph_view->get_node_tree_map(parent_to_children_names);
        
        graph_sort_algorithm->set_root_node(this->graph_view->get_graph_node(root_task->get_name()));
        graph_sort_algorithm->set_parent_to_children(parent_to_children);
    
        godot::HashMap<BTGraphNode*, godot::Vector2> result = graph_sort_algorithm->get_arranged_nodes_position();
    
        for (const auto& key_value : result)
        {
            CHECK_VECTORS_EQ(key_value.key->get_position_offset(), key_value.value);
        }
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Task change name")
    {
        test_task_name_change();
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Task change class")
    {
        test_task_class_change();
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Task changing class doesn't change parents and children")
    {
        test_task_class_change_keeps_connections();
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Test saving tree.")
    {
        godot::Vector2 old_position = root->get_position_offset();
        graph_view->set_node_position(root_task->get_name(), old_position + godot::Vector2(-10, 0));
        BehaviourTree* new_tree = memnew(BehaviourTree);
        editor->set_behaviour_tree(new_tree);

        CHECK_EQ(graph_view->get_connection_list().size(), 0);
        CHECK_EQ(graph_view->get_child_count(), initial_child_count);

        editor->set_behaviour_tree(tree);

        CHECK_EQ(graph_view->get_connection_list().size(), 6);
        CHECK_EQ(graph_view->get_child_count(), initial_child_count + tree->get_task_count());
        CHECK_EQ(graph_view->get_graph_node(root_task->get_name())->get_position_offset(), old_position + godot::Vector2(-10, 0));
        memdelete(new_tree);
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Copy tasks")
    {
        test_copy_nodes_request();
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Copy tasks with connections")
    {
        test_copy_nodes_copies_connections();
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Paste tasks")
    {
        test_paste_nodes_request();
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Paste tasks with connections")
    {
        test_paste_nodes_with_connections_request();
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

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Task change name")
    {
        test_task_name_change();

        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();
        
        CHECK_EQ(root_task->get_custom_name(), "");
        CHECK_EQ(root->get_title(), "");

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        CHECK_EQ(root_task->get_custom_name(), "root3K");
        CHECK_EQ(root->get_title(), "root3K");
    }

    TEST_CASE_FIXTURE(BTGraphEditorReadyTreeFixture, "Task change class")
    {
        test_task_class_change_keeps_connections();

        godot::Ref<BTTask> new_task = tree->get_root_task()->get_child(0);

        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();
        
        assert_task_changed("BTTask", child1_task, new_task);

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        assert_task_changed("BTCondition", new_task, child1_task);
    }

    TEST_CASE_FIXTURE(BTGraphEditorFixture, "Test node deletion")
    {
        test_node_deletion();

        godot::UndoRedo* undo = get_undo(tree);
        undo->undo();

        assert_node_before_deletion();
        godot::Ref<BTTask> node1_task = tree->get_task(2);

        godot::UndoRedo* redo = get_redo(tree);
        redo->redo();

        assert_node_deleted(node1_task);
    }

}