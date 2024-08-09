#ifndef BT_MACROS_HPP
#define BT_MACROS_HPP

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>

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

#define CREATE_GETTER_SETTER_STRINGTYPE(type, name, member) /******************************************************************************************************************/\
    void set_##name(const type& value) { this->member = value; }                                                                                                                \
    _FORCE_INLINE_ type get_##name() const { return this->member; }

#define CREATE_GETTER_SETTER_STRING(name, member) /****************************************************************************************************************************/\
    CREATE_GETTER_SETTER_STRINGTYPE(godot::String, name, member)

#define CREATE_GETTER_SETTER_STRING_DEFAULT(member) /**************************************************************************************************************************/\
    CREATE_GETTER_SETTER_STRING(member, member)

#define CREATE_GETTER_SETTER_STRINGNAME(name, member) /************************************************************************************************************************/\
    CREATE_GETTER_SETTER_STRINGTYPE(godot::StringName, name, member)

#define CREATE_GETTER_SETTER_STRINGNAME_DEFAULT(member) /**********************************************************************************************************************/\
    CREATE_GETTER_SETTER_STRING(member, member)

#define CREATE_GETTER_SETTER_POSITIVE(type, name, member) /********************************************************************************************************************/\
    void set_##name(type value) {                                                                                                                                               \
        this->member = godot::Math::clamp(value, type(0), value);                                                                                                               \
    }                                                                                                                                                                           \
    _FORCE_INLINE_ type get_##name() const { return this->member; }

#define CREATE_GETTER_SETTER_POSITIVE_DEFAULT(type, member) /******************************************************************************************************************/\
    CREATE_GETTER_SETTER_POSITIVE(type, member, member)


// include bt_task after so it can use definitions for getter_setter
#include "behaviour_tree/tasks/bt_task.hpp"

#define TASK_FAIL() /**********************************************************************************************************************************************************/\
    return BTTask::Status::FAILURE

#define TASK_SUCCEED() /*******************************************************************************************************************************************************/\
    return BTTask::Status::SUCCESS

#define TASK_COMPLAIN_COND(condition, message) /*******************************************************************************************************************************/\
    if ((condition) && (this->is_complain_enabled()))                                                                                                                                        \
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


#endif /* BT_MACROS */