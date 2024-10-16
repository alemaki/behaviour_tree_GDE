#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include "behaviour_tree/utils/utils.hpp"

class TestRunner : public godot::Node
{
   GDCLASS(TestRunner, godot::Node);

public:
   static bool g_error_called;
   static bool currently_testing_error;


private:
   bool tests_ran = false;
   godot::Ref<godot::SceneTreeTimer> test_timer;

public:
   void run(const char* filter);
   void run_runtime();
   void run_editor();
   virtual void _ready() override;

protected:
    static void _bind_methods();

};

#endif // TEST_RUNNER_HPP