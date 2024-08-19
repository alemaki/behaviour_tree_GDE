#ifndef BT_TEST_UTILS_HPP
#define BT_TEST_UTILS_HPP

#include <doctest.h>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "tests/test_utils/test_runner.hpp"

void simulate(godot::Node* node, int frames = 1);

double get_node_delta(godot::Node* node);
double get_current_engine_delta();

bool vectors_almost_eq(godot::Vector3 vec1, godot::Vector3 vec2, bool complain = true);
bool vectors_almost_ne(godot::Vector3 vec1, godot::Vector3 vec2, bool complain = true);

#ifdef CHECK_VECTORS_EQ
#undef CHECK_VECTORS_EQ
#endif
#define CHECK_VECTORS_EQ(vec1, vec2) CHECK(::vectors_almost_eq(vec1, vec2, true));

#ifdef CHECK_VECTORS_NE
#undef CHECK_VECTORS_NE
#endif
#define CHECK_VECTORS_NE(vec1, vec2) CHECK(::vectors_almost_ne(vec1, vec2, true));


#ifdef REQUIRE_VECTORS_EQ
#undef REQUIRE_VECTORS_EQ
#endif
#define REQUIRE_VECTORS_EQ(vec1, vec2) REQUIRE(::vectors_almost_eq(vec1, vec2, true));

#ifdef REQUIRE_VECTORS_NE
#undef REQUIRE_VECTORS_NE
#endif
#define REQUIRE_VECTORS_NE(vec1, vec2) REQUIRE(::vectors_almost_ne(vec1, vec2, true));

#endif /* BT_TEST_UTILS_HPP */