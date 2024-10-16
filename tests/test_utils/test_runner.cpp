#include "test_runner.hpp"
#include <sstream>
#include <stdexcept>

#include <doctest.h>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>

GDExtensionInterfacePrintError original_gdextension_interface_print_error = nullptr;
GDExtensionInterfacePrintErrorWithMessage original_gdextension_interface_print_error_with_message = nullptr;

bool TestRunner::g_error_called = false;
bool TestRunner::currently_testing_error = false;

void custom_gdextension_interface_print_error(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    TestRunner::g_error_called = true;
    if (!(TestRunner::currently_testing_error))
    {
        godot::UtilityFunctions::printerr(godot::String("[Godot Error: ") + p_description + "] at " + p_file + ": " + p_function + " line(" + godot::itos(p_line) + ")");
    }
}

void custom_gdextension_interface_print_error_with_message(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, GDExtensionBool p_editor_notify)
{
    TestRunner::g_error_called = true;
    if (!(TestRunner::currently_testing_error))
    {
        godot::UtilityFunctions::printerr(godot::String("[Godot Error: ") + p_description + "] at " + p_file + ": " + p_function + " line(" + godot::itos(p_line) + ")");
        godot::UtilityFunctions::printerr(godot::String("[Message: ") + p_message + "]");
    }
}

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

    /* save for later */
    original_gdextension_interface_print_error = godot::internal::gdextension_interface_print_error;
    original_gdextension_interface_print_error_with_message = godot::internal::gdextension_interface_print_error_with_message;
    godot::internal::gdextension_interface_print_error = custom_gdextension_interface_print_error;
    godot::internal::gdextension_interface_print_error_with_message = custom_gdextension_interface_print_error_with_message;


    int res = context.run();
    godot::UtilityFunctions::print(output_stream.str().c_str());

    /* Restore the original functions */
    godot::internal::gdextension_interface_print_error = original_gdextension_interface_print_error;
    godot::internal::gdextension_interface_print_error_with_message = original_gdextension_interface_print_error_with_message;

    if (context.shouldExit())
    {
        /* Nothing to do. */
    }
}

void TestRunner::run_runtime()
{
    this->run("--test-suite-exclude=*[editor]*");
}

void TestRunner::run_editor()
{
    this->run("--test-suite=*[editor]*");
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
        godot::UtilityFunctions::print("\n\nEditor tests running.\n\n");
        this->run_editor();
    }
}

void TestRunner::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(godot::D_METHOD("run_runtime"), &TestRunner::run_runtime);
}