#include "test_runner.hpp"
#include <doctest.h>


godot::SceneTree* test_runner_scene_tree;

godot::SceneTree* get_test_runner_scene_tree()
{
    return test_runner_scene_tree;
}

void TestRunner::run()
{
    doctest::Context context;
    context.applyCommandLine(0, nullptr);
    this->set_scene_tree();
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
    ERR_FAIL_COND(test_runner_scene_tree == nullptr);
}

void TestRunner::_process(double delta)
{
    if (!tests_ran)
    {
        tests_ran = true;
        this->run();
    }
}


void TestRunner::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("run"), &TestRunner::run);
}
