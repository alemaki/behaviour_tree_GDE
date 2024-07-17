#ifndef BT_TEST_UTILS_HPP
#define BT_TEST_UTILS_HPP

#include <doctest.h>
#include<godot_cpp/classes/node.hpp>

void simulate(godot::Node* node,int frames = 1);

double get_node_delta(godot::Node* node);
double get_current_engine_delta();

bool vectors_almost_equal(godot::Vector2 vec1, godot::Vector2 vec2);

#endif /* BT_TEST_UTILS_HPP */