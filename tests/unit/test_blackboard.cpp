#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include <godot_cpp/classes/node2d.hpp>

#include "blackboard/blackboard.hpp"

TEST_SUITE("BlackboardBasic")
{
    TEST_CASE("Set and get variable")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        
        blackboard->set_var("health", 100);
        CHECK_EQ((int64_t)blackboard->get_var("health"), 100);
    }

    TEST_CASE("Set and get bool variable")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);

        blackboard->set_var("is_active", true);
        CHECK((bool)blackboard->get_var("is_active"));
    }

    TEST_CASE("Set and get array variable")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);

        godot::Array positions;
        positions.push_back(Vector2(0, 0));
        positions.push_back(Vector2(1, 1));

        blackboard->set_var("positions", positions);
        CHECK_EQ(blackboard->get_var("positions"), positions);
    }

    TEST_CASE("Set and get Node2D pointer")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        godot::Node2D *node = memnew(godot::Node2D);

        blackboard->set_var("node", node);
        CHECK_EQ(blackboard->get_var("node").operator Object*(), node);

        memdelete(node);
    }

    TEST_CASE("Has variable")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        
        blackboard->set_var("health", 100);
        CHECK(blackboard->has_var("health"));
        CHECK_FALSE(blackboard->has_var("mana"));
    }

    TEST_CASE("Erase variable")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        
        blackboard->set_var("health", 100);
        blackboard->erase_var("health");
        CHECK_FALSE(blackboard->has_var("health"));
    }

    TEST_CASE("Clear variables")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        
        blackboard->set_var("health", 100);
        blackboard->set_var("mana", 50);
        blackboard->clear();
        CHECK_FALSE(blackboard->has_var("health"));
        CHECK_FALSE(blackboard->has_var("mana"));
    }

    TEST_CASE("List variables")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        
        blackboard->set_var("health", 100);
        blackboard->set_var("mana", 50);
        
        TypedArray<StringName> vars = blackboard->list_vars();
        CHECK_EQ(vars.size(), 2);
        CHECK(vars.has("health"));
        CHECK(vars.has("mana"));
    }

    TEST_CASE("Get variables as dictionary")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        
        blackboard->set_var("health", 100);
        blackboard->set_var("mana", 50);
        
        Dictionary vars = blackboard->get_vars_as_dict();
        CHECK_EQ(vars.size(), 2);
        CHECK_EQ(vars["health"], godot::Variant(100));
        CHECK_EQ(vars["mana"], godot::Variant(50));
    }

    TEST_CASE("Get non-existent variable")
    {
        godot::Ref<Blackboard> blackboard = memnew(Blackboard);
        
        CHECK_EQ(blackboard->get_var("non_existent", 42, false), godot::Variant(42));
    }
}