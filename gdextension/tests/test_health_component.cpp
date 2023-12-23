#ifndef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif //DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#ifndef TEST_HEALTH_COMPONENT_H
#define TEST_HEALTH_COMPONENT_H

#include <godot_cpp/classes/ref.hpp>
#include "components/health/health_component.hpp"
#include "doctest/doctest.h"

namespace TestHealthComponent
{
TEST_CASE("[HealthComponent] Basic set.")
{
    godot::Ref<HealthComponent> health_component(memnew(HealthComponent));

    health_component->set_max_hp(20);
    health_component->set_current_hp(20);
    REQUIRE(health_component->get_max_hp() == 20);
    REQUIRE(health_component->get_current_hp() == 20);

    health_component->set_max_hp(-20);
    REQUIRE(health_component->get_max_hp() >= 0);

    health_component->set_current_hp(-20);
    REQUIRE(health_component->get_current_hp() >= 0);


    health_component->set_max_hp(10);
    health_component->set_current_hp(20);
    REQUIRE(health_component->get_current_hp() <= 10);
}  

TEST_CASE("[HealthComponent] Take damage.")
{
    godot::Ref<HealthComponent> health_component(memnew(HealthComponent));


    health_component->set_max_hp(20);
    health_component->set_current_hp(20);
    REQUIRE(health_component->get_max_hp() == 20);
    REQUIRE(health_component->get_current_hp() == 20);

    SUBCASE("Normal damage.")
    {
        health_component->take_damage(10);
        CHECK(health_component->get_current_hp() == 10);
        health_component->take_damage(5);
        CHECK(health_component->get_current_hp() == 5);
    }

    SUBCASE("Overflow damage.")
    {
        health_component->take_damage(25);
        CHECK(health_component->get_current_hp() == 0);
    }

    SUBCASE("Negative damage.")
    {
        
        health_component->take_damage(-20);
        CHECK(health_component->get_current_hp() == 20);
    }
}

TEST_CASE("[HealthComponent] Test signals.")
{
    godot::Ref<HealthComponent> health_component(memnew(HealthComponent));

    // TODO. Signal testing.
}

}

#endif //TEST_HEALTH_COMPONENT_H