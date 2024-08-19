#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include <godot_cpp/classes/node3d.hpp>

#include "blackboard/blackboard.hpp"

struct BlackboardFixture
{
    godot::Ref<Blackboard> blackboard;
    BlackboardFixture() : blackboard(memnew(Blackboard)) {}
    ~BlackboardFixture(){}
};

TEST_SUITE("BlackboardBasic")
{
    TEST_CASE_FIXTURE(BlackboardFixture, "Set and get variable")
    {
        blackboard->set_var("health", 100);
        CHECK_EQ((int64_t)blackboard->get_var("health"), 100);
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Set and get bool variable")
    {
        blackboard->set_var("is_active", true);
        CHECK((bool)blackboard->get_var("is_active"));
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Set and get array variable")
    {
        godot::Array positions;
        positions.push_back(Vector3(0, 0, 0));
        positions.push_back(Vector3(1, 1, 0));

        blackboard->set_var("positions", positions);
        CHECK_EQ(blackboard->get_var("positions"), positions);
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Set and get Node3D pointer")
    {
        godot::Node3D *node = memnew(godot::Node3D);

        blackboard->set_var("node", node);
        CHECK_EQ(blackboard->get_var("node").operator Object*(), node);

        memdelete(node);
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Has variable")
    {
        blackboard->set_var("health", 100);
        CHECK(blackboard->has_var("health"));
        CHECK_FALSE(blackboard->has_var("mana"));
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Erase variable")
    {
        blackboard->set_var("health", 100);
        blackboard->erase_var("health");
        CHECK_FALSE(blackboard->has_var("health"));
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Clear variables")
    {
        blackboard->set_var("health", 100);
        blackboard->set_var("mana", 50);
        blackboard->clear();
        CHECK_FALSE(blackboard->has_var("health"));
        CHECK_FALSE(blackboard->has_var("mana"));
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "List variables")
    {
        blackboard->set_var("health", 100);
        blackboard->set_var("mana", 50);
        
        TypedArray<StringName> vars = blackboard->list_vars();
        CHECK_EQ(vars.size(), 2);
        CHECK(vars.has("health"));
        CHECK(vars.has("mana"));
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Get variables as dictionary")
    {
        blackboard->set_var("health", 100);
        blackboard->set_var("mana", 50);
        
        Dictionary vars = blackboard->get_vars_as_dict();
        CHECK_EQ(vars.size(), 2);
        CHECK_EQ(vars["health"], godot::Variant(100));
        CHECK_EQ(vars["mana"], godot::Variant(50));
    }

    TEST_CASE_FIXTURE(BlackboardFixture, "Get non-existent variable")
    {
        CHECK_EQ(blackboard->get_var("non_existent", 42, false), godot::Variant(42));
    }
}