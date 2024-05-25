#include "test_runner.hpp"
#include <doctest.h>

#include <godot_cpp/variant/utility_functions.hpp>

godot::SceneTree* test_runner_scene_tree;

godot::Node* get_scene_root()
{
    return test_runner_scene_tree->get_current_scene();
}

void TestRunner::run()
{
    doctest::Context context;
    context.applyCommandLine(0, nullptr);
    /* TODO: doctest allows to change the stdout. Try to redirect to godot.*/
    int res = context.run();

    if (context.shouldExit())
    {
        return;
    }
}

void TestRunner::set_scene_tree()
{
    test_runner_scene_tree = this->get_tree();
}

void TestRunner::_ready()
{
    this->set_scene_tree();
    ERR_FAIL_COND(test_runner_scene_tree == nullptr);
    godot::Node* current_scene = get_scene_root();
    /* Ensure tests run only in executed scene */
    if (current_scene != nullptr) 
    {
        this->run();
    }
}

void TestRunner::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("run"), &TestRunner::run);
}
