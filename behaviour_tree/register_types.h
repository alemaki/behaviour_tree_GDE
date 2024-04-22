#ifndef BEHAVIOUR_TREE_REGISTER_TYPES_H
#define BEHAVIOUR_TREE_REGISTER_TYPES_H

#include "godot_cpp/core/class_db.hpp"

using namespace godot;

void initialize_behaviour_tree_module(ModuleInitializationLevel p_level);
void initialize_test_module(ModuleInitializationLevel p_level);
void uninitialize_behaviour_tree_module(ModuleInitializationLevel p_level);

#endif // BEHAVIOUR_TREE_REGISTER_TYPES_H
