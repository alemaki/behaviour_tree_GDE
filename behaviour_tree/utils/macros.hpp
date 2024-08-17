#ifndef BT_MACROS_HPP
#define BT_MACROS_HPP

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/class_db.hpp>



/* GETTERS AND SETTERS */
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

/* BINDINGS */
#define BIND_GETTER_SETTER(class, name, parameter) /***************************************************************************************************************************/\
    ClassDB::bind_method(D_METHOD("set_" #name, #parameter), &class::set_##name);                                                                                               \
    ClassDB::bind_method(D_METHOD("get_" #name), &class::get_##name)

#define BIND_GETTER_SETTER_DEFAULT(class, name) /******************************************************************************************************************************/\
    BIND_GETTER_SETTER(class, name, name)

#define BIND_GETTER_SETTER_BOOL(class, name, parameter) /**********************************************************************************************************************/\
    ClassDB::bind_method(D_METHOD("set_" #name, #parameter), &class::set_##name);                                                                                               \
    ClassDB::bind_method(D_METHOD("is_" #name), &class::is_##name)

#define BIND_GETTER_SETTER_BOOL_DEFAULT(class, name) /*************************************************************************************************************************/\
    BIND_GETTER_SETTER_BOOL(class, name, name)

#define BIND_PROPERTY(name, variant_type, member) /****************************************************************************************************************************/\
    ADD_PROPERTY(PropertyInfo(Variant::variant_type, #member), "set_" #name, "get_" #name)

#define BIND_PROPERTY_DEFAULT(variant_type, member) /**************************************************************************************************************************/\
    BIND_PROPERTY(member, variant_type, member)

#define BIND_PROPERTY_BOOL(name, member) /*************************************************************************************************************************************/\
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, #member), "set_" #name, "is_" #name)

#define BIND_PROPERTY_BOOL_DEFAULT(member) /***********************************************************************************************************************************/\
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, #member), "set_" #member, "is_" #member)

#define BIND_GETTER_SETTER_PROPERTY(class, name, parameter, variant_type, member) /********************************************************************************************/\
    BIND_GETTER_SETTER(class, name, parameter);                                                                                                                                 \
    BIND_PROPERTY(name, variant_type, member)

#define BIND_GETTER_SETTER_PROPERTY_DEFAULT(class, variant_type, member) /*****************************************************************************************************/\
    BIND_GETTER_SETTER_PROPERTY(class, member, member, variant_type, member)

#define BIND_GETTER_SETTER_PROPERTY_BOOL(class, name, parameter, member) /*****************************************************************************************************/\
    BIND_GETTER_SETTER_BOOL(class, name, parameter);                                                                                                                            \
    BIND_PROPERTY_BOOL(name, member)

#define BIND_GETTER_SETTER_PROPERTY_BOOL_DEFAULT(class, member) /**************************************************************************************************************/\
    BIND_GETTER_SETTER_PROPERTY_BOOL(class, member, member, member)

#define BIND_PROPERTY_OBJECT(name, member, property_hint, hint_str, property_usage, property_class) /**************************************************************************/\
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, #member, property_hint, hint_str, property_usage, #property_class), "set_" #name, "get_" #name)

#define BIND_PROPERTY_OBJECT_DEFAULT(member, property_hint, hint_str, property_usage, property_class) /************************************************************************/\
    BIND_PROPERTY_OBJECT(member, member, property_hint, hint_str, property_usage, property_class)

#define BIND_PROPERTY_OBJECT_NO_HINT(member) /*********************************************************************************************************************************/\
    BIND_PROPERTY_OBJECT(member, member, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR, )

#define BIND_GETTER_SETTER_PROPERTY_OBJECT(class, name, parameter, member, property_hint, hint_str, property_usage, property_class) /******************************************/\
    BIND_GETTER_SETTER(class, name, parameter);                                                                                                                                 \
    BIND_PROPERTY_OBJECT(name, member, property_hint, hint_str, property_usage, #property_class)

#define BIND_GETTER_SETTER_PROPERTY_OBJECT_DEFAULT(class, member, property_hint, hint_str, property_usage, property_class) /***************************************************/\
    BIND_GETTER_SETTER_DEFAULT(class, member);                                                                                                                                  \
    BIND_PROPERTY_OBJECT_DEFAULT(member, property_hint, hint_str, property_usage, property_class)

#define BIND_GETTER_SETTER_PROPERTY_OBJECT_NO_HINT(class, member) /************************************************************************************************************/\
    BIND_GETTER_SETTER_DEFAULT(class, member);                                                                                                                                  \
    BIND_PROPERTY_OBJECT_NO_HINT(member)

// include bt_task after so it can use definitions for getter_setter
#include "behaviour_tree/tasks/bt_task.hpp"

#define TASK_FAIL() /**********************************************************************************************************************************************************/\
    return BTTask::Status::FAILURE

#define TASK_SUCCEED() /*******************************************************************************************************************************************************/\
    return BTTask::Status::SUCCESS

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

#define TASK_COMPLAIN_COND(condition, message) /*******************************************************************************************************************************/\
    if ((condition) && (this->is_complain_enabled()))                                                                                                                           \
    {                                                                                                                                                                           \
        godot::UtilityFunctions::printerr(message);                                                                                                                             \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_FAIL_COND_COMP_MSG(condition, message) /**************************************************************************************************************************/\
    TASK_COMPLAIN_COND(condition, message);                                                                                                                                     \
    TASK_FAIL_COND(condition)


#endif /* BT_MACROS */