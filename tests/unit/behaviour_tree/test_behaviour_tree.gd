extends GutTest

func after_all():
	assert_no_new_orphans("Memory leak..")

func test_set_and_get_description():
	var tree = add_child_autofree(BehaviourTree.new())
	var description = "Test Behaviour Tree"
	tree.set_description(description)
	assert_eq(tree.get_description(), description, "Description should be set correctly.")

func test_add_and_get_task():
	var tree = add_child_autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	tree.add_task_by_ref(task)
	var task_id = tree.get_task_id(task); 
	assert_eq(tree.get_task_by_id(task_id), task, "Task should be retrievable by ID.")

func test_has_task():
	var tree = add_child_autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	assert_false(tree.has_task(task), "Tree should not have the task.")
	tree.add_task_by_ref(task)
	assert_true(tree.has_task(task), "Tree should have the added task.")
	var fake_task = autofree(BTTask.new())
	assert_false(tree.has_task(fake_task), "Tree should not have a task that wasn't added.")

func test_remove_task():
	var tree = add_child_autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())

	tree.add_task_by_ref(task)
	var task_id = tree.get_task_id(task)
	tree.remove_task(task_id)

	assert_false(tree.has_task(task), "Task should be removed from the tree.")
	assert_eq(tree.get_task_by_id(task_id), null, "Removed task should not be retrievable.")

	tree.add_task_by_ref(task)
	tree.remove_task_by_ref(task)
	assert_false(tree.has_task(task), "Task should be removed by ref from the tree.")

func test_remove_task_count():
	var tree = add_child_autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	var task1 = autofree(BTTask.new())
	var task2 = autofree(BTTask.new())
	tree.add_task_by_ref(task)
	tree.add_task_by_ref(task1)
	tree.add_task_by_ref(task2)
	tree.remove_task_by_ref(task)
	assert_true(tree.get_tasks().size() == 2, "There should be 2 tasks left.")
	tree.remove_task_by_ref(task1)
	assert_true(tree.get_tasks().size() == 1, "There should be 1 tasks left.")
	tree.remove_task_by_ref(task2)
	assert_true(tree.get_tasks().size() == 0, "There should be 0 tasks left.")


func test_connect_and_disconnect_tasks():
	var tree = add_child_autofree(BehaviourTree.new())
	var parent_task = autofree(BTTask.new())
	var child_task = autofree(BTTask.new())
	tree.add_task_by_ref(parent_task)
	tree.add_task_by_ref(child_task)

	tree.connect_tasks(parent_task, child_task, 0)
	assert_true(parent_task.has_child(child_task), "Parent task should have the child task after connecting.")
	assert_eq(child_task.get_parent(), parent_task, "Child task's parent should be the parent task after connecting.")

	tree.disconnect_tasks(parent_task, child_task)
	assert_false(parent_task.has_child(child_task), "Parent task should not have the child task after disconnecting.")
	assert_eq(child_task.get_parent(), null, "Child task's parent should be null after disconnecting.")

func test_clear_tasks():
	var tree = add_child_autofree(BehaviourTree.new())
	var task1 = autofree(BTTask.new())
	var task2 = autofree(BTTask.new())
	tree.add_task_by_ref(task1)
	tree.add_task_by_ref(task2)
	tree.clear_tasks()
	assert_eq(tree.get_tasks().size(), 0, "Tree should have no tasks after clearing.")
	assert_eq(tree.get_root_task(), null, "Root task should be null after clearing tasks.")

func test_set_and_get_root_task():
	var tree = add_child_autofree(BehaviourTree.new())
	var root_task = autofree(BTTask.new())
	tree.add_task_by_ref(root_task)
	tree.set_root_task(root_task)
	assert_eq(tree.get_root_task(), root_task, "Root task shouldn't be set.")

func test_detach_task():
	var tree = add_child_autofree(BehaviourTree.new())
	var root_task = autofree(BTTask.new())
	var child_task = autofree(BTTask.new())
	tree.add_task_by_ref(root_task)
	tree.add_task_by_ref(child_task)
	tree.connect_tasks(root_task, child_task, 0)
	tree.detach_task_by_ref(child_task)
	assert_false(root_task.has_child(child_task), "Child task should be detached from root task.")
	assert_eq(child_task.get_parent(), null, "Detached child task should have no parent.")

func test_set_tasks_of_parent():
	var tree = add_child_autofree(BehaviourTree.new())
	var parent_task = autofree(BTTask.new())
	var child_task1 = autofree(BTTask.new())
	var child_task2 = autofree(BTTask.new())
	tree.add_task_by_ref(parent_task)
	tree.add_task_by_ref(child_task1)
	tree.add_task_by_ref(child_task2)
	var children_array = [child_task1, child_task2]
	tree.set_tasks_of_parent(parent_task, children_array)
	var children = parent_task.get_children()
	assert_eq(children.size(), 2, "Parent task should have 2 children.")
	assert_eq(children[0], child_task1, "First child should be child_task1.")
	assert_eq(children[1], child_task2, "Second child should be child_task2.")

func test_root_task_initially_null():
	var tree = add_child_autofree(BehaviourTree.new())
	assert_eq(tree.get_root_task(), null, "Root task should initially be null.")

func test_added_task_becomes_root():
	var tree = add_child_autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	var task1 = autofree(BTTask.new())
	tree.add_task_by_ref(task)
	assert_eq(tree.get_root_task(), task, "First task should become root.")
	tree.add_task_by_ref(task1)
	assert_eq(tree.get_root_task(), task, "First task should stay root.")

func test_set_root_task():
	var tree = add_child_autofree(BehaviourTree.new())
	var root_task1 = autofree(BTTask.new())
	var root_task2 = autofree(BTTask.new())
	tree.add_task_by_ref(root_task1)
	tree.add_task_by_ref(root_task2)
	tree.set_root_task(root_task2)
	assert_eq(tree.get_root_task(), root_task2, "Root task should be the last one set.")

func test_remove_root_task():
	var tree = add_child_autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	var task1 = autofree(BTTask.new())
	tree.add_task_by_ref(task)
	tree.add_task_by_ref(task1)
	var root_task = tree.get_root_task()
	tree.remove_task_by_ref(root_task)
	assert_false(tree.get_root_task() == root_task, "Root task should be changed.")
	assert_false(tree.get_root_task() == null, "Root task should change and not null")

func test_get_valid_id_increment():
	var tree = add_child_autofree(BehaviourTree.new())
	var first_id = tree.get_valid_id()
	var task = autofree(BTTask.new())
	tree.add_task_by_ref(task)
	var second_id = tree.get_valid_id()
	assert_eq(second_id, first_id + 1, "Each call to get_valid_id should return a unique, incrementing ID.")

func test_task_hierarchy():
	var tree = add_child_autofree(BehaviourTree.new())
	var root_task = autofree(BTTask.new())
	var child_task1 = autofree(BTTask.new())
	var child_task2 = autofree(BTTask.new())
	var grandchild_task = autofree(BTTask.new())

	tree.add_task_by_ref(root_task)
	tree.add_task_by_ref(child_task1)
	tree.add_task_by_ref(child_task2)
	tree.add_task_by_ref(grandchild_task)

	tree.connect_tasks(root_task, child_task1, 0)
	tree.connect_tasks(root_task, child_task2, 1)
	tree.connect_tasks(child_task1, grandchild_task, 0)

	assert_true(root_task.has_child(child_task1), "Root task should have child_task1.")
	assert_true(root_task.has_child(child_task2), "Root task should have child_task2.")
	assert_true(child_task1.has_child(grandchild_task), "child_task1 should have grandchild_task.")
	assert_eq(grandchild_task.get_root(), root_task, "Root of grandchild_task should be root_task.")
	assert_eq(grandchild_task.get_parent(), child_task1, "Parent of grandchild_task should be child_task1.")

func test_set_custom_name_task_by_ref():
	var behaviour_tree = add_child_autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	behaviour_tree.add_task_by_ref(task)
	var new_name = "New Task Name"
	behaviour_tree.set_custom_name_task_by_ref(task, new_name)

	assert_eq(task.get_custom_name(), new_name, "Task custom name should be updated.")

func test_swap_root_task_in():
	var behaviour_tree = add_child_autofree(BehaviourTree.new())
	var old_task = autofree(BTTask.new())
	var new_task = autofree(BTTask.new())

	behaviour_tree.add_task_by_ref(old_task)

	var child_task = BTTask.new()
	behaviour_tree.add_task_by_ref(child_task)
	behaviour_tree.connect_tasks(old_task, child_task, 0)

	behaviour_tree.swap_task_in(old_task, new_task)

	assert_true(behaviour_tree.has_task(new_task), "New task should be in the tree.")
	assert_false(behaviour_tree.has_task(old_task), "Old task should be removed from the tree.")
	assert_eq(behaviour_tree.get_root_task(), new_task, "New task should be the root now.")
	assert_eq(new_task.get_children().size(), 1, "New task should inherit old task's children.")
	assert_true(new_task.has_child(child_task), "Child task should be correctly reassigned.")
	assert_eq(child_task.get_parent(), new_task, "Child task should have correctly reassigned parent.")


func test_swap_non_root_task_in():
	var behaviour_tree = add_child_autofree(BehaviourTree.new())
	var old_task = autofree(BTTask.new())
	var new_task = autofree(BTTask.new())
	var root_task = autofree(BTTask.new())
	var child1 = autofree(BTTask.new())
	var child2 = autofree(BTTask.new())

	behaviour_tree.add_task_by_ref(root_task)
	behaviour_tree.add_task_by_ref(old_task)
	behaviour_tree.add_task_by_ref(child1)
	behaviour_tree.add_task_by_ref(child2)

	behaviour_tree.connect_tasks(old_task, child1, 0)
	behaviour_tree.connect_tasks(old_task, child2, 1)
	behaviour_tree.connect_tasks(root_task, old_task, 0)

	behaviour_tree.swap_task_in(old_task, new_task)

	assert_true(behaviour_tree.has_task(new_task), "New task should be in the tree.")
	assert_false(behaviour_tree.has_task(old_task), "Old task should be removed from the tree.")
	assert_eq(behaviour_tree.get_root_task(), root_task, "Old sroot should stay root.")
	assert_eq(new_task.get_children().size(), 2, "New task should inherit old task's children.")
	assert_true(new_task.has_child(child1), "Child task should be correctly reassigned.")
	assert_true(new_task.has_child(child1), "Child task should be correctly reassigned.")
	assert_eq(child1.get_parent(), new_task, "Child task should have correctly reassigned parent.")
	assert_eq(child2.get_parent(), new_task, "Child task should have correctly reassigned parent.")
	assert_eq(new_task.get_parent(),  root_task, "New task should inherit parent.")
	assert_eq(root_task.get_children().size(), 1, "Root task should still have only one child.")
	assert_true(root_task.has_child(new_task), "Root task should have updated child.")



