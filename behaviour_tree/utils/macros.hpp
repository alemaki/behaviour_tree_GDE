#ifndef BT_MACROS_HPP
#define BT_MACROS_HPP

#include <godot_cpp/variant/utility_functions.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"

#define TASK_FAIL() /**********************************************************************************************************************************************************/\
    return BTTask::Status::FAILURE;

#define TASK_SUCCEED() /*******************************************************************************************************************************************************/\
    return BTTask::Status::SUCCESS;

#define TASK_FAIL_COND(condition) /********************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0);

#define TASK_SUCCEED_COND(condition) /*****************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::SUCCESS;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0);

#define TASK_FAIL_COND_MSG(condition, message) /*******************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        godot::UtilityFunctions::print(message);                                                                                                                                \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0);

#define TASK_FAIL_COND_COMP_MSG(condition, message) /**************************************************************************************************************************/\
    if (this->complain)                                                                                                                                                         \
    {                                                                                                                                                                           \
        TASK_FAIL_COND_MSG(condition, message)                                                                                                                                  \
    }                                                                                                                                                                           \
    else                                                                                                                                                                        \
    {                                                                                                                                                                           \
        TASK_FAIL_COND(condition)                                                                                                                                               \
    }

#endif /* BT_MACROS */