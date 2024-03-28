extends GutTest

func after_all():
	mock_behaviour_tree.free()
	assert_no_new_orphans("Memory leak.")

const mock_behaviour_tree_path: String = "res://tests/mock/mock_behaviour_tree.tscn"
var mock_behaviour_tree: BehaviourTree = preload(mock_behaviour_tree_path).instantiate();


func test_mock_tree_valid_for_testing():
	assert_ne(mock_behaviour_tree, null, "Mock tree should exist.")
	assert_ne(mock_behaviour_tree.get_root_task(), null, "Mock tree should have at least one task.")
	assert_eq(mock_behaviour_tree.get_root_task().get_child_count(), 2, "Mock tree should have at least 2 children of root.")
	assert_eq(mock_behaviour_tree.get_root_task().get_child(0).get_child_count(), 0, "Root children should be empty.")
	assert_eq(mock_behaviour_tree.get_root_task().get_child(1).get_child_count(), 0, "Root children should be empty.")

func test_load_behavior_tree():
	var subtree_task: BTSubtree = BTSubtree.new()
	subtree_task.set_file_path(mock_behaviour_tree_path)

	var sub_behaviour_tree: BehaviourTree = autofree(subtree_task.load_behavior_tree())

	assert_ne(sub_behaviour_tree, null, "Behaviour tree should be loaded from scene file.")
	assert_ne(sub_behaviour_tree.get_root_task(), null, "Loaded behaviour tree should have a root task.")

func test_clone_subtree_task():
	var subtree_task: BTSubtree = BTSubtree.new()
	subtree_task.set_file_path(mock_behaviour_tree_path)
	
	var tree_root_task: BTTask = mock_behaviour_tree.get_root_task()
	var cloned_root_task: BTTask = autofree(subtree_task.clone())

	assert_ne(cloned_root_task, null, "Cloned task should not be null.")
	
	assert_ne(cloned_root_task, tree_root_task, "New tree should be clone.")
	
	assert_eq(tree_root_task.get_child_count(), cloned_root_task.get_child_count(), "Cloned trees should be the same by their parameters")
	assert_eq(tree_root_task.get_class(), cloned_root_task.get_class(), "Cloned trees should be the same by their parameters")

	var child1: BTTask = tree_root_task.get_child(0);
	var child2: BTTask =  tree_root_task.get_child(1);
	var cloned_child1: BTTask = cloned_root_task.get_child(0);
	var cloned_child2: BTTask = cloned_root_task.get_child(1);

	assert_ne(child1, cloned_child1, "Children should be clones.")
	assert_eq(child1.get_child_count(), cloned_child1.get_child_count(), "Cloned trees should be the same by their parameters")
	assert_eq(child1.get_class(), cloned_child1.get_class(), "Cloned trees should be the same by their parameters")

	assert_ne(child2, cloned_child2, "Children should be clones.")
	assert_eq(child2.get_child_count(), cloned_child2.get_child_count(), "Cloned trees should be the same by their parameters")
	assert_eq(child2.get_class(), cloned_child2.get_class(), "Cloned trees should be the same by their parameters")
