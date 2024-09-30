#include "test_runner.hpp"
#include <sstream>

#include <doctest.h>

#include <godot_cpp/variant/utility_functions.hpp>

void TestRunner::run(const char* filter)
{
    const char* argv[] = {
        "", 
        "--test-suite-exclude=*[deprecated]*", 
        filter, 
        //"--success",
        //"--duration",
        //"--order-by=name"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    doctest::Context context(argc, argv);

    std::stringstream output_stream;

    context.setCout(&output_stream);

    int res = context.run();
    godot::UtilityFunctions::print(output_stream.str().c_str());

    if (context.shouldExit())
    {
        /* Nothing to do. */
    }
}

void TestRunner::run_runtime()
{
    this->run("--test-suite-exclude=*[editor]*");
}

void TestRunner::_ready()
{
    godot::Node* current_scene_root = ::get_scene_root();
    /* Ensure editor tests do not run in scene that is played */
    if (current_scene_root != nullptr) 
    {
        /* The reason for this is to give the engine some time to pass a few frames, so the physics delta may be different than 0.
         *  */
        /* Approximate 1 frame at 60 FPS */
        this->test_timer = ::get_scene_tree()->create_timer(1.0 / 60.0);
        this->test_timer->connect("timeout", callable_mp(this, &TestRunner::run_runtime), CONNECT_ONE_SHOT);
    }
    else 
    {
        //deprecated
        //godot::UtilityFunctions::print("\n\nEditor tests running.\n\n");
        //this->run_editor("--test-suite=*[editor]*");
    }
}

void TestRunner::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(godot::D_METHOD("run_runtime"), &TestRunner::run_runtime);
}