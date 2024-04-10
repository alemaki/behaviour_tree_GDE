#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include <godot_cpp/classes/node.hpp>
#include <doctest.h>
class TestRunner : public godot::Node
{
    GDCLASS(TestRunner, godot::Node);
    
public:
   void run();
   virtual void _ready() override;

protected:
    static void _bind_methods();

};

#endif // TEST_RUNNER_HPP