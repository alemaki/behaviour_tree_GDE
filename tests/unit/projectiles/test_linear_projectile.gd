extends GutTest
		
func after_all():
	assert_no_new_orphans("Memory leak.")

func test_linear_projectile_basic():
	var projectile: LinearProjectile = add_child_autofree(LinearProjectile.new())
	
	assert_almost_eq(projectile.get_direction(), Vector2.ZERO, Vector2(0.001, 0.001), "Initial direction should be Vector2.ZERO")
	assert_almost_eq(projectile.get_speed(), 0.0, 0.001, "Initial speed should be 0")
	
	var direction: Vector2 = Vector2.RIGHT + Vector2.UP
	var speed: float = 100
	projectile.set_direction(direction)
	projectile.set_speed(speed)
	
	assert_almost_eq(projectile.get_direction(), direction.normalized(), Vector2(0.001, 0.001), "Direction should be set to normalized value")
	assert_almost_eq(projectile.get_speed(), speed, 0.001, "Speed should be set to 100")

func test_linear_projectile_movement():
	var projectile: LinearProjectile = add_child_autofree(LinearProjectile.new())
	
	projectile.set_direction(Vector2.RIGHT)
	projectile.set_speed(100)
	projectile.set_physics_process(true)
	#can't call simulate so I have to do it manually
	projectile._physics_process(0.1)
	var expected_position: Vector2 = Vector2(10, 0)
	assert_almost_eq(projectile.get_position(), expected_position, Vector2(0.001, 0.001), "Projectile position should be updated after physics process")
