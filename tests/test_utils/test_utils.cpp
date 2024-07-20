#include "test_utils.hpp"

#include <godot_cpp/classes/engine.hpp>

void simulate_frame_physics_process(godot::Node* node, float delta)
{
	godot::Array children = node->get_children();
	int size = children.size();
	for (int i = 0; i < size; i++)
	{
		simulate_frame_physics_process(godot::Object::cast_to<godot::Node>(children[i]), delta);
	}
	node->_physics_process(delta);
}

void simulate_frame_process(godot::Node* node, float delta)
{
	godot::Array children = node->get_children();
	int size = children.size();
	for (int i = 0; i < size; i++)
	{
		simulate_frame_process(godot::Object::cast_to<godot::Node>(children[i]), delta);
	}
	node->_process(delta);

}

void simulate(godot::Node* node, int frames)
{
	double delta = get_node_delta(node);
	if (delta == 0)
	{
		delta = get_current_engine_delta();
	}
	ERR_FAIL_COND(delta == 0);
	for (int i = 0; i < frames; ++i)
    {
		simulate_frame_process(node, delta);
	}
	for (int i = 0; i < frames; ++i)
    {
		simulate_frame_physics_process(node, delta);
	}
}

double get_current_engine_delta()
{
	return  godot::Engine::get_singleton()->is_in_physics_frame() ? ::get_scene_root()->get_physics_process_delta_time() : ::get_scene_root()->get_process_delta_time();
}

double get_node_delta(godot::Node* node)
{
	return  godot::Engine::get_singleton()->is_in_physics_frame() ? node->get_physics_process_delta_time() : node->get_process_delta_time();
}

bool vectors_almost_eq(godot::Vector2 vec1, godot::Vector2 vec2, bool complain)
{
	bool result = doctest::Approx(vec1.x) == vec2.x && doctest::Approx(vec1.y) == vec2.y;

	if ((!result) && (complain))
	{
		godot::UtilityFunctions::printerr("Test error.");
		godot::UtilityFunctions::printerr(vec1, " != ", vec2);
	}
    return result;
}


bool vectors_almost_ne(godot::Vector2 vec1, godot::Vector2 vec2, bool complain)
{
	bool result = doctest::Approx(vec1.x) != vec2.x || doctest::Approx(vec1.y) != vec2.y;

	if ((!result) && (complain))
	{
		godot::UtilityFunctions::printerr("Test error.");
		godot::UtilityFunctions::printerr(vec1, " == ", vec2);
	}
    return result;
}