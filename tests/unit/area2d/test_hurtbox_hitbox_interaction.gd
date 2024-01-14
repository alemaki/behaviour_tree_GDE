extends GutTest

func after_all():
	assert_no_new_orphans("Memory leak.")

func test_hitbox_and_hurtbox_interaction():
	# Add to tree so _ready is called
	var hitbox: Hitbox = add_child_autoqfree(Hitbox.new())
	var hurtbox: Hurtbox = add_child_autoqfree(Hurtbox.new())
	
	assert_has_signal(hitbox, "hit_hurtbox")
	assert_has_signal(hurtbox, "hurtbox_hit")
	
	watch_signals(hitbox)
	watch_signals(hurtbox)
	
	hitbox.set_monitoring (true)
	hitbox.area_entered.emit(hurtbox)
	
	# Check if signals were emitted wiith right info
	assert_signal_emitted(hitbox, "area_entered")
	assert_signal_emitted_with_parameters(hitbox, "hit_hurtbox", [hurtbox])
	assert_signal_emitted_with_parameters(hurtbox, "hurtbox_hit", [hitbox])
