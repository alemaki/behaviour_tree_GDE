#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

godot::SceneTree* get_scene_tree();
godot::Node* get_scene_root();

class TestRunner : public godot::Node
{
   GDCLASS(TestRunner, godot::Node);

private:
   bool tests_ran = false;
   godot::Ref<godot::SceneTreeTimer> test_timer;

public:
   void run(const char* filter);
   void run_runtime();
   virtual void _ready() override;

protected:
    static void _bind_methods();

};

#endif // TEST_RUNNER_HPP