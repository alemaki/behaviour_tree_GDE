extends GutTest

func after_all():
	assert_no_new_orphans("Memory leak..")

func test_set_and_get_description():
	var tree = autofree(BehaviourTree.new())
	var description = "Test Behaviour Tree"
	tree.set_description(description)
	assert_eq(tree.get_description(), description, "Description should be set correctly.")

func test_add_and_get_task():
	var tree = autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	var task_id = tree.get_valid_id()
	tree.add_task(task_id, task)
	assert_eq(tree.get_task_by_id(task_id), task, "Task should be retrievable by ID.")

func test_has_task():
	var tree = autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	assert_false(tree.has_task(task), "Tree should not have the task.")
	tree.add_task_by_ref(task)
	assert_true(tree.has_task(task), "Tree should have the added task.")
	var fake_task = autofree(BTTask.new())
	assert_false(tree.has_task(fake_task), "Tree should not have a task that wasn't added.")

func test_remove_task():
	var tree = autofree(BehaviourTree.new())
	var task = autofree(BTTask.new())
	var task_id = tree.get_valid_id()
	tree.add_task(task_id, task)
	tree.remove_task(task_id)
	assert_false(tree.has_task(task), "Task should be removed from the tree.")
	assert_eq(tree.get_task_by_id(task_id), null, "Removed task should not be retrievable.")

func test_connect_and_disconnect_tasks():
	var tree = autofree(BehaviourTree.new())
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
	var tree = autofree(BehaviourTree.new())
	var task1 = autofree(BTTask.new())
	var task2 = autofree(BTTask.new())
	tree.add_task_by_ref(task1)
	tree.add_task_by_ref(task2)
	tree.clear_tasks()
	assert_eq(tree.get_tasks().size(), 0, "Tree should have no tasks after clearing.")
	assert_eq(tree.get_root_task(), null, "Root task should be null after clearing tasks.")

func test_set_and_get_root_task():
	var tree = autofree(BehaviourTree.new())
	var root_task = autofree(BTTask.new())
	tree.add_task_by_ref(root_task)
	tree.set_root_task(root_task)
	assert_eq(tree.get_root_task(), root_task, "Root task shouldn't be set.")

func test_detach_task():
	var tree = autofree(BehaviourTree.new())
	var root_task = autofree(BTTask.new())
	var child_task = autofree(BTTask.new())
	tree.add_task_by_ref(root_task)
	tree.add_task_by_ref(child_task)
	tree.connect_tasks(root_task, child_task, 0)
	tree.detach_task_by_ref(child_task)
	assert_false(root_task.has_child(child_task), "Child task should be detached from root task.")
	assert_eq(child_task.get_parent(), null, "Detached child task should have no parent.")

func test_set_tasks_of_parent():
	var tree = autofree(BehaviourTree.new())
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
	var tree = autofree(BehaviourTree.new())
	assert_eq(tree.get_root_task(), null, "Root task should initially be null.")

func test_double_set_root_task():
	var tree = autofree(BehaviourTree.new())
	var root_task1 = autofree(BTTask.new())
	var root_task2 = autofree(BTTask.new())
	tree.add_task_by_ref(root_task1)
	tree.add_task_by_ref(root_task2)
	assert_eq(tree.get_root_task(), root_task1, "Root task should be set.")
	tree.set_root_task(root_task2)
	assert_eq(tree.get_root_task(), root_task2, "Root task should be the last one set.")

func test_get_valid_id_increment():
	var tree = autofree(BehaviourTree.new())
	var first_id = tree.get_valid_id()
	var task = autofree(BTTask.new())
	tree.add_task_by_ref(task)
	var second_id = tree.get_valid_id()
	assert_eq(second_id, first_id + 1, "Each call to get_valid_id should return a unique, incrementing ID.")

func test_task_hierarchy():
	var tree = autofree(BehaviourTree.new())
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
