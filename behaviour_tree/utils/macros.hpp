#ifndef BT_MACROS_HPP
#define BT_MACROS_HPP

#include <godot_cpp/variant/utility_functions.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"

#define TASK_FAIL_COND(condition) /********************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }

#define TASK_FAIL_COND_MSG(condition, message) /*******************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        godot::UtilityFunctions::print(message);                                                                                                                                \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }

#define TASK_SUCCEED_COND(condition) /*****************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::SUCCESS;                                                                                                                                         \
    }

#endif /* BT_MACROS */