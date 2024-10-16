#ifndef BT_TEST_UTILS_HPP
#define BT_TEST_UTILS_HPP

#include <doctest.h>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "tests/test_utils/test_runner.hpp"

void simulate(godot::Node* node, int frames = 1);

double get_node_delta(godot::Node* node);
double get_current_engine_delta();

#ifdef CHECK_VECTORS_EQ
#undef CHECK_VECTORS_EQ
#endif
#define CHECK_VECTORS_EQ(vec1, vec2) CHECK_MESSAGE((vec1).is_equal_approx(vec2), (std::string(godot::String(vec1).utf8().get_data()) + " != " + godot::String(vec2).utf8().get_data()))

#ifdef CHECK_VECTORS_NE
#undef CHECK_VECTORS_NE
#endif
#define CHECK_VECTORS_NE(vec1, vec2) CHECK_MESSAGE(!((vec1).is_equal_approx(vec2)), (std::string(godot::String(vec1).utf8().get_data()) + " != " + godot::String(vec2).utf8().get_data()))


#ifdef REQUIRE_VECTORS_EQ
#undef REQUIRE_VECTORS_EQ
#endif
#define REQUIRE_VECTORS_EQ(vec1, vec2) REQUIRE_MESSAGE((vec1).is_equal_approx(vec2), (std::string(godot::String(vec1).utf8().get_data()) + " != " + godot::String(vec2).utf8().get_data()))

#ifdef REQUIRE_VECTORS_NE
#undef REQUIRE_VECTORS_NE
#endif
#define REQUIRE_VECTORS_NE(vec1, vec2) REQUIRE_MESSAGE(!((vec1).is_equal_approx(vec2)), (std::string(godot::String(vec1).utf8().get_data()) + " != " + godot::String(vec2).utf8().get_data()))

#define CHECK_GODOT_ERROR(expression) /************************************************************************************************************************************/\
TestRunner::g_error_called = false;                                                                                                                                         \
TestRunner::currently_testing_error = true;                                                                                                                                 \
expression;                                                                                                                                                                 \
CHECK(TestRunner::g_error_called);                                                                                                                                          \
TestRunner::currently_testing_error = false;                                                                                                                        


#define REQUIRE_GODOT_ERROR(expression) /**********************************************************************************************************************************/\
TestRunner::g_error_called = false;                                                                                                                                         \
TestRunner::currently_testing_error = true;                                                                                                                                 \
expression;                                                                                                                                                                 \
REQUIRE(TestRunner::g_error_called);                                                                                                                                        \
TestRunner::currently_testing_error = false;

#endif /* BT_TEST_UTILS_HPP */