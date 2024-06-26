#ifndef BT_TEST_UTILS_HPP
#define BT_TEST_UTILS_HPP

#include<godot_cpp/classes/node.hpp>

void simulate(godot::Node* node, float delta = 1.0f, int frames = 1);

#endif /* BT_TEST_UTILS_HPP */