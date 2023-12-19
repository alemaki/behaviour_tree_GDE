extends GutTest
		
func after_all():
	assert_no_new_orphans("Memory leak.")

func test_linear_projectile():
	var projectile: LinearProjectile = add_child_autoqfree(LinearProjectile.new())
	
	assert_eq(projectile.get_direction(), Vector2.ZERO, "Initial direction should be Vector2.ZERO")
	assert_eq(projectile.get_speed(), 0, "Initial speed should be 0")
	
	var direction = Vector2.RIGHT + Vector2.UP
	var speed = 100
	projectile.set_direction(direction)
	projectile.set_speed(speed)
	
	assert_eq(projectile.get_direction(), direction.normalized(), "Direction should be set to normalized value")
	assert_eq(projectile.get_speed(), speed, "Speed should be set to 100")
	
	projectile.set_direction(Vector2.RIGHT)
	simulate(projectile, 2, 0.1)
	var expected_position = Vector2(10, 0)
	assert_eq(projectile.get_position(), expected_position, "Projectile position should be updated after physics process")
