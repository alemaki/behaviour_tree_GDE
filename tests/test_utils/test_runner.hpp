#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

godot::Node* get_scene_root();

class TestRunner : public godot::Node
{
    GDCLASS(TestRunner, godot::Node);

private:
   bool tests_ran = false;

private:
   void set_scene_tree();

public:
   void run();
   virtual void _ready() override;

protected:
    static void _bind_methods();

};

#endif // TEST_RUNNER_HPP