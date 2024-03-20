extends GutTest

func after_all():
	assert_no_new_orphans("Memory leak..")

func test_task_clone_basic():
	var task: BTTask = autofree(BTTask.new())
	var cloned_task: BTTask = task.clone()

	assert_ne(cloned_task, null, "A new task should be created.")
	assert_ne(cloned_task,  task, "A different task should be created.")
	assert_eq(cloned_task.get_class(), task.get_class(), "The new task should be of the same type.")
	assert_eq(cloned_task.get_actor(), task.get_actor(), "The new task should have the same attributes.")
	assert_eq(cloned_task.get_status(), task.get_status(), "The new task should have the same attributes.")

func test_task_clone_keeps_attributes():
	var task_prob: BTTask = autofree(BTProbability.new())
	task_prob.set_run_chance(0.5)
	var cloned_task: BTTask = task_prob.clone()

	assert_ne(cloned_task, task_prob, "A different task should be created.")
	assert_eq(cloned_task.get_class(), task_prob.get_class(), "The new task should be of the same type.")
	assert_eq(cloned_task.get_run_chance(), task_prob.get_run_chance(), "The attribute should be the same.")

func test_task_cloning_children():
	var task: BTTask = autofree(BTTask.new())
	var child1: BTInvert = autofree(BTInvert.new())
	var child2: BTSequence = autofree(BTSequence.new())
	task.add_child(child1)
	task.add_child(child2)
	
	var cloned_task: BTTask = task.clone()

	assert_ne(cloned_task, task, "A different task should be created.")
	assert_eq(cloned_task.get_child_count(), 2, "Cloned task should have the same number of children.")
	assert_false(cloned_task.has_child(child1), "Cloned task should have different children.")
	assert_false(cloned_task.has_child(child2), "Cloned task should have different children.")
	assert_eq(cloned_task.get_children()[0].get_class(), child1.get_class(), "Cloned children should have the same type and be in same order")
	assert_eq(cloned_task.get_children()[1].get_class(), child2.get_class(), "Cloned children should have the same type and be in same order")
