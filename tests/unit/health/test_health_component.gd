extends GutTest

func after_all():
	assert_no_new_orphans("Memory leak.")
	

func test_health_component_basics():
	var health_component: HealthComponent = autoqfree(HealthComponent.new())
	
	assert_eq(health_component.get_max_hp(), 0, "Initial max_hp should be 0.")
	assert_eq(health_component.get_current_hp(), 0, "Initial current_hp should be 0.")
	
	health_component.set_max_hp(100)
	assert_eq(health_component.get_max_hp(), 100, "Max_hp should be set to 100.")
	
	health_component.set_max_hp(-10)
	assert_eq(health_component.get_max_hp(), 0, "Max_hp should not be set to negative value.")
	
	health_component.set_current_hp(-10)
	assert_eq(health_component.get_current_hp(), 0, "Current_hp should not be set to negative value.")
	
	health_component.set_max_hp(10)
	health_component.set_current_hp(10)
	health_component.set_max_hp(5)
	assert_eq(health_component.get_current_hp(), 5, "Current_hp should be between 0 and max_hp always.")
	
	health_component.set_max_hp(10)
	health_component.set_current_hp(20)
	assert_eq(health_component.get_current_hp(), 10, "Current_hp should not be set to value bigger than max.")
	
	health_component.set_current_hp(-1)
	assert_eq(health_component.get_current_hp(), 0, "Current_hp should not be set to negative value.")

func test_health_component_take_damage():
	var health_component: HealthComponent = autoqfree(HealthComponent.new())
	
	health_component.set_max_hp(100)
	health_component.set_current_hp(50)
	
	health_component.take_damage(20)
	assert_eq(health_component.get_current_hp(), 30, "Current_hp should be decreased by 20.")
	
	health_component.take_damage(40)
	assert_eq(health_component.get_current_hp(), 0, "Current_hp should be decreased to 0 and not less.")
	
	health_component.take_damage(-20)
	assert_eq(health_component.get_current_hp(), 0, "Current_hp should shouldn't change on negative damage.")	
