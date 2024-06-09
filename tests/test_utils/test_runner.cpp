#include "test_runner.hpp"
#include <doctest.h>

#include <godot_cpp/variant/utility_functions.hpp>

godot::SceneTree* test_runner_scene_tree;


godot::Node* get_scene_root()
{
    return test_runner_scene_tree->get_current_scene();
}

void TestRunner::run(const char* filter)
{
    doctest::Context context;
    const char* argv[] = {"", filter};
    context.applyCommandLine(2, argv);
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
    /* Ensure editor tests do not run in scene that is played */
    if (current_scene != nullptr) 
    {
        this->run("~[editor]");
    }
    else 
    {
        this->run("[editor]");
    }
}

void TestRunner::_bind_methods()
{
    using namespace godot;

}