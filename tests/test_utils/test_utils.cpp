#include "test_utils.hpp"

void simulate(godot::Node* node, float delta, int frames)
{
	for (int i = 0; i < frames; ++i)
    {
		node->_process(delta);
	}
}
