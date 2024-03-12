extends GutTest

func after_all():
	assert_no_new_orphans("Memory leak.")

func test_character_input_component():
	# Add object as child so processing may be enabled.
	var character_input_component: CharacterInputComponent = add_child_autofree(CharacterInputComponent.new())
	
	assert_eq(character_input_component.get_direction_input(), Vector2.ZERO, "Initial direction input should be Vector2.ZERO")
	
	Input.action_press("ui_right")
	# Use simulate instead of wait for frames for faster performance
	simulate(character_input_component, 1, 0)
	assert_eq(character_input_component.get_direction_input(), Vector2.RIGHT, "Direction input should be Vector2.RIGHT after processing")
	
	Input.action_press("ui_left")
	simulate(character_input_component, 1, 0)
	assert_eq(character_input_component.get_direction_input(), Vector2.ZERO, "Direction input should be Vector2.ZERO when both actions pressed.")
	
	Input.action_release("ui_right")
	Input.action_release("ui_left")
	
	# Test remaining.
	Input.action_press("ui_up")
	simulate(character_input_component, 1, 0)
	assert_eq(character_input_component.get_direction_input(), Vector2.UP, "")
	Input.action_release("ui_up")
	
	Input.action_press("ui_down")
	simulate(character_input_component, 1, 0)
	assert_eq(character_input_component.get_direction_input(), Vector2.DOWN, "")
	
	#Test bidirections
	Input.action_press("ui_right")
	simulate(character_input_component, 1, 0)
	assert_eq(character_input_component.get_direction_input(), (Vector2.DOWN + Vector2.RIGHT).normalized(), "")
	Input.action_release("ui_right")
	Input.action_release("ui_down")
