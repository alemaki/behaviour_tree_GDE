#include "test_runner.hpp"
#include <doctest.h>


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

void TestRunner::_ready()
{
    this->run();
}


void TestRunner::_bind_methods()
{
    using namespace godot;

    ClassDB::bind_method(D_METHOD("run"), &TestRunner::run);
}
