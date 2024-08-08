#ifndef BT_MACROS_HPP
#define BT_MACROS_HPP

#include <godot_cpp/variant/utility_functions.hpp>

#include "behaviour_tree/tasks/bt_task.hpp"

#define TASK_FAIL() /**********************************************************************************************************************************************************/\
    return BTTask::Status::FAILURE

#define TASK_SUCCEED() /*******************************************************************************************************************************************************/\
    return BTTask::Status::SUCCESS

#define TASK_COMPLAIN_COND(condition, message) /*******************************************************************************************************************************/\
    if ((condition) && (this->complain))                                                                                                                                        \
    {                                                                                                                                                                           \
        godot::UtilityFunctions::printerr(message);                                                                                                                             \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_FAIL_COND(condition) /********************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_SUCCEED_COND(condition) /*****************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::SUCCESS;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_FAIL_COND_MSG(condition, message) /*******************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        godot::UtilityFunctions::print(message);                                                                                                                                \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_FAIL_COND_COMP_MSG(condition, message) /**************************************************************************************************************************/\
    TASK_COMPLAIN_COND(condition, message);                                                                                                                                     \
    TASK_FAIL_COND(condition)



#define CREATE_GETTER_SETTER(type, name, member) /*****************************************************************************************************************************/\
    void set_##name(type value) { this->member = value; }                                                                                                                       \
    _FORCE_INLINE_ type get_##name() const { return this->member; }

#define CREATE_GETTER_SETTER_DEFAULT(type, member) /***************************************************************************************************************************/\
    CREATE_GETTER_SETTER(type, member, member)

#define CREATE_GETTER_SETTER_BOOL(name, member) /******************************************************************************************************************************/\
    void set_##name(bool value) { this->member = value; }                                                                                                                       \
    _FORCE_INLINE_ bool is_##name() const { return this->member; }

#define CREATE_GETTER_SETTER_BOOL_DEFAULT(member) /****************************************************************************************************************************/\
    CREATE_GETTER_SETTER_BOOL(member, member)

#define CREATE_GETTER_SETTER_POSITIVE(type, name, member) /*****************************************************************************************************************************/\
    void set_##name(type value) {                                                                                                                                                        \
        value = godot::Math::clamp<type>(value, 0, value);                                                                                                                               \
        this->member = value;                                                                                                                                                            \
    }                                                                                                                                                                                    \
    _FORCE_INLINE_ type get_##name() const { return this->member; }

#define CREATE_GETTER_SETTER_POSITIVE_DEFAULT(type, member) /***************************************************************************************************************************/\
    CREATE_GETTER_SETTER(type, member, member)

#endif /* BT_MACROS */