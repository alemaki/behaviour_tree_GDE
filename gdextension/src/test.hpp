#ifndef TEST1_HPP
#define TEST1_HPP

#include "godot_cpp/classes/node2d.hpp"

using namespace godot;

class Test : public Node2D
{
    GDCLASS(Test, Node2D);

protected:
    static void _bind_methods(){};

};

#endif // TEST1_HPP