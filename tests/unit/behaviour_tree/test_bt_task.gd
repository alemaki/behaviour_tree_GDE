extends GutTest

func after_all():
	assert_no_new_orphans("Memory leak..")

func test_bt_task_basic():
	var root_task: BTTask = autofree(BTTask.new())
	
	assert_eq(root_task.get_status(), 0, "Initial status should be FRESH.")
	
	var actor_node: Node = autofree(Node.new())
	root_task.set_actor(actor_node)
	assert_eq(root_task.get_actor(), actor_node, "Actor should be set correctly.")
	
	assert_null(root_task.get_parent(), "Parent should be null.")


func test_bt_task_child_addition_and_array():
	var root_task: BTTask = autofree(BTTask.new())
	var child1: BTTask = autofree(BTTask.new())
	var child2: BTTask = autofree(BTTask.new())
	var child3: BTTask = autofree(BTTask.new())

	root_task.add_child(child1)
	root_task.add_child(child2)
	
	var children = root_task.get_children()
	assert_eq(children[0], child1, "First child should be child1.")
	assert_eq(children[1], child2, "Second child should be child2.")
	
	root_task.add_child(child3)
	
	children = root_task.get_children()
	assert_eq(children[0], child1, "First child should be child1.")
	assert_eq(children[1], child2, "Second child should be child2.")
	assert_eq(children[2], child3, "Third child should be child3.")

func test_bt_task_has_child():
	var root_task: BTTask = autofree(BTTask.new())
	var child_task: BTTask = autofree(BTTask.new())
	
	assert_false(root_task.has_child(child_task), "Root task should not have child task before adding.")
	root_task.add_child(child_task)
	
	assert_true(root_task.has_child(child_task), "Root task should have child task after adding.")

	var other_task: BTTask = autofree(BTTask.new())
	assert_false(root_task.has_child(other_task), "Root task should not have other task.")

func test_bt_task_child_count():
	var root_task: BTTask = autofree(BTTask.new())
	var child1: BTTask = autofree(BTTask.new())
	var child2: BTTask = autofree(BTTask.new())
	var child3: BTTask = autofree(BTTask.new())

	root_task.add_child(child1)
	assert_eq(root_task.get_child_count(), 1, "Child count should be 1 after adding children.")
	
	root_task.add_child(child2)
	assert_eq(root_task.get_child_count(), 2, "Child count should be 2 after adding children.")
	
	root_task.add_child(child3)
	assert_eq(root_task.get_child_count(), 3, "Child count should be 3 after adding children.")

func test_bt_task_child_twice_addtition():
	var root_task: BTTask = autofree(BTTask.new())
	var child1: BTTask = autofree(BTTask.new())
	var child2: BTTask = autofree(BTTask.new())

	root_task.add_child(child1)
	root_task.add_child(child2)
	root_task.add_child(child1)
	root_task.add_child(child2)
	assert_eq(root_task.get_child_count(), 2, "Children shouldn't be added twice..")

func test_bt_task_child_addition_at_index():
	var root_task: BTTask = autofree(BTTask.new())
	var child1: BTTask = autofree(BTTask.new())
	var child2: BTTask = autofree(BTTask.new())
	var child3: BTTask = autofree(BTTask.new())

	root_task.add_child(child1)
	root_task.add_child_at_index(child2, 0)
	
	var children = root_task.get_children()
	assert_eq(children[0], child2, "First child should be child2.")
	assert_eq(children[1], child1, "Second child should be child1.")
	
	root_task.add_child_at_index(child3, 1)
	
	children = root_task.get_children()
	assert_eq(children[0], child2, "First child should be child2.")
	assert_eq(children[1], child3, "Second child should be child3.")
	assert_eq(children[2], child1, "Third child should be child1.")

func test_bt_task_child_removal():
	var root_task: BTTask = autofree(BTTask.new())
	var child1: BTTask = autofree(BTTask.new())
	var child2: BTTask = autofree(BTTask.new())
	var child3: BTTask = autofree(BTTask.new())

	root_task.add_child(child1)
	root_task.add_child(child2)
	root_task.add_child(child3)
	
	root_task.remove_child(child1)
	assert_false(root_task.has_child(child1), "Removed child should be child1.")
	assert_eq(root_task.get_child_count(), 2, "Child count should be 2 after removing child.")
	assert_ne(root_task, child1.get_parent(), "Child should be not have parent after removal.")
	assert_ne(root_task, child1.get_root(), "Child should be root after removal.")
	
	var children = root_task.get_children()
	assert_eq(children[0], child2, "First child should be child2.")
	assert_eq(children[1], child3, "Second child should be child3.")
	
	root_task.remove_child(child3)
	assert_false(root_task.has_child(child3), "Removed child should be child3.")
	assert_ne(root_task, child3.get_parent(), "Child should be not have parent after removal.")
	assert_ne(root_task, child3.get_root(), "Child should be root after removal.")
	assert_true(root_task.has_child(child2), "Remaining child should be child2.")

func test_bt_task_child_twice_removal():
	var root_task: BTTask = autofree(BTTask.new())
	var child1: BTTask = autofree(BTTask.new())
	var child2: BTTask = autofree(BTTask.new())

	root_task.add_child(child1)
	root_task.add_child(child2)
	assert_eq(root_task.get_child_count(), 2, "Child count should be 2.")
	root_task.remove_child(child1)
	root_task.remove_child(child1)
	assert_eq(root_task.get_child_count(), 1, "Child should be removed once.")


func test_bt_task_child_removal_at_index():
	var root_task: BTTask = autofree(BTTask.new())
	var child1: BTTask = autofree(BTTask.new())
	var child2: BTTask = autofree(BTTask.new())
	var child3: BTTask = autofree(BTTask.new())

	root_task.add_child(child1)
	root_task.add_child(child2)
	root_task.add_child(child3)
	
	root_task.remove_child_at_index(1)
	assert_false(root_task.has_child(child2), "Removed child should be child2.")
	assert_ne(root_task, child2.get_parent(), "Child should be not have parent after removal.")
	assert_ne(root_task, child2.get_root(), "Child should be root after removal.")
	
	assert_eq(root_task.get_child_count(), 2, "Child count should be 2 after removing child.")
	
	var children = root_task.get_children()
	assert_eq(children[0], child1, "First child should be child1.")
	assert_eq(children[1], child3, "Second child should be child3.")
	
	root_task.remove_child_at_index(0)
	assert_false(root_task.has_child(child1), "Removed child should be child1.")
	assert_ne(root_task, child1.get_parent(), "Child should be not have parent after removal.")
	assert_ne(root_task, child1.get_root(), "Child should be root after removal.")

func test_bt_task_get_parent_after_children_added():
	var root_task: BTTask = autofree(BTTask.new())
	var child_task: BTTask = autofree(BTTask.new())
	var grandchild_task: BTTask = autofree(BTTask.new())
	
	root_task.add_child(child_task)
	child_task.add_child(grandchild_task)

	assert_eq(grandchild_task.get_parent(), child_task, "Parent of grandchild task should be child task.")

	var another_child_task: BTTask = autofree(BTTask.new())
	root_task.add_child(another_child_task)
	assert_eq(another_child_task.get_parent(), root_task, "Parent of another child task should be root.")
	assert_eq(child_task.get_parent(), root_task, "Parent of child task should be root task after another child has been added.")

func test_bt_task_is_root():
	var root_task: BTTask = autofree(BTTask.new())
	var child_task: BTTask = autofree(BTTask.new())
	var grandchild_task: BTTask = autofree(BTTask.new())

	root_task.add_child(child_task)
	child_task.add_child(grandchild_task)

	assert_true(root_task.is_root(), "Root task should be identified as root.")
	assert_false(child_task.is_root(), "Child task should not be identified as root.")
	assert_false(grandchild_task.is_root(), "Grandchild task should not be identified as root.")

func test_bt_task_get_root():
	var root_task: BTTask = autofree(BTTask.new())
	var child_task: BTTask = autofree(BTTask.new())
	var grandchild_task: BTTask = autofree(BTTask.new())

	root_task.add_child(child_task)
	child_task.add_child(grandchild_task)
	
	var grandchild_root = grandchild_task.get_root()
	assert_eq(root_task, grandchild_root.get_root(), "Root of grandchild task should be root task.")
	
	var child_root = child_task.get_root()
	assert_eq(root_task, child_root.get_root(), "Root of child task should be root task.")
