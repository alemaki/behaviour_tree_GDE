#ifndef BT_MACROS_HPP
#define BT_MACROS_HPP

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/class_db.hpp>

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


#define BIND_GETTER_SETTER(class, name, parameter) /***************************************************************************************************************************/\
    ClassDB::bind_method(D_METHOD("set_" #name, #parameter), &class::set_##name);                                                                                               \
    ClassDB::bind_method(D_METHOD("get_" #name), &class::get_##name);

#define BIND_GETTER_SETTER_DEFAULT(class, name) /******************************************************************************************************************************/\
    ClassDB::bind_method(D_METHOD("set_" #name, #name), &class::set_##name);                                                                                                    \
    ClassDB::bind_method(D_METHOD("get_" #name), &class::get_##name);

#define BIND_PROPERTY(name, variant_type, member) /****************************************************************************************************************************/\
    ADD_PROPERTY(PropertyInfo(Variant::variant_type, #member), "set_" #name, "get_" #name);

#define BIND_PROPERTY_DEFAULT(variant_type, member) /**************************************************************************************************************************/\
    ADD_PROPERTY(PropertyInfo(Variant::variant_type, #member), "set_" #member, "get_" #member);

#define BIND_GETTER_SETTER_PROPERTY(class, name, parameter, variant_type, member) /********************************************************************************************/\
    BIND_GETTER_SETTER(class, name, parameter)                                                                                                                                  \
    BIND_PROPERTY(name, variant_type, member)

#define BIND_GETTER_SETTER_PROPERTY_DEFAULT(class, variant_type, member) /*****************************************************************************************************/\
    BIND_GETTER_SETTER_DEFAULT(class, member)                                                                                                                                   \
    BIND_PROPERTY_DEFAULT(variant_type, member)

// include bt_task after so it can use definitions for getter_setter
#include "behaviour_tree/tasks/bt_task.hpp"

#define TASK_FAIL() /**********************************************************************************************************************************************************/\
    return BTTask::Status::FAILURE

#define TASK_SUCCEED() /*******************************************************************************************************************************************************/\
    return BTTask::Status::SUCCESS

#define TASK_COMPLAIN_COND(condition, message) /*******************************************************************************************************************************/\
    if ((condition) && (this->is_complain_enabled()))                                                                                                                           \
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