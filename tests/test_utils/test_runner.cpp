#include "test_runner.hpp"
#include <doctest.h>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

godot::SceneTree* test_runner_scene_tree;


godot::SceneTree* get_scene_tree()
{
    return godot::Object::cast_to<godot::SceneTree>(godot::Engine::get_singleton()->get_main_loop());
}

godot::Node* get_scene_root()
{
    return get_scene_tree()->get_current_scene();
;
}

void TestRunner::run(const char* filter)
{
    const char* argv[] = {"", "--test-suite-exclude=*[deprecated]*" ,filter};
    doctest::Context context(3, argv);
    /* TODO: doctest allows to change the stdout. Try to redirect to godot.*/
    int res = context.run();
    godot::UtilityFunctions::print("Finished tests.");
    if (context.shouldExit())
    {
        /* Nothing to do. */
    }
}


void TestRunner::set_scene_tree()
{
    test_runner_scene_tree = this->get_tree();
}

void TestRunner::_ready()
{
    godot::Node* current_scene = get_scene_root();
    /* Ensure editor tests do not run in scene that is played */
    if (current_scene != nullptr) 
    {
        this->run("--test-suite-exclude=*[editor]*");
    }
    else 
    {
        //deprecated
        //godot::UtilityFunctions::print("\n\nEditor tests running.\n\n");
        //this->run("--test-suite=*[editor]*");
    }
}

void TestRunner::_bind_methods()
{
    using namespace godot;

}