#ifndef BT_TASK_HPP
#define BT_TASK_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>

#include "blackboard/blackboard.hpp"

#include "behaviour_tree/utils/macros.hpp"

class BTTask : public godot::Resource
{
    GDCLASS(BTTask, godot::Resource);

public:
    enum Status 
    {
        FRESH,
        RUNNING,
        SUCCESS,
        FAILURE,
    };

private:
    Status status = BTTask::Status::FRESH;
    BTTask* parent = nullptr;
    godot::String custom_name = "";
    godot::Node* actor = nullptr;
    godot::Ref<Blackboard> blackboard = nullptr;
    godot::Vector<godot::Ref<BTTask>> children = {};
    bool debugging = false;

protected:
    virtual void _setup();
    virtual void _enter();
    virtual void _exit();
    virtual Status _tick(double delta);

public:
    BTTask();

    CREATE_GETTER_SETTER_BOOL_DEFAULT(debugging);
    CREATE_GETTER_SETTER_STRING_DEFAULT(custom_name);
    CREATE_GETTER_SETTER_DEFAULT(godot::Node*, actor);
    CREATE_GETTER_SETTER_DEFAULT(godot::Ref<Blackboard>, blackboard);
    CREATE_GETTER_SETTER_DEFAULT(BTTask::Status, status);

    void set_parent(godot::Ref<BTTask> parent);
    _FORCE_INLINE_ godot::Ref<BTTask> get_parent() const
    {
        return godot::Ref<BTTask>(this->parent);
    }
    godot::Array get_children() const;
    void _set_children(const godot::Array& children);
    
    _FORCE_INLINE_ int get_child_count() const
    {
        return this->children.size();
    }
    _FORCE_INLINE_ godot::Ref<BTTask> get_child(int index) const
    {
        return godot::Ref<BTTask>(this->children[index]);
    }
    bool is_root() const;
    
	godot::Ref<BTTask> get_root() const;

	void add_child(godot::Ref<BTTask> child);
	void add_child_at_index(godot::Ref<BTTask> child, int index);
	void remove_child(godot::Ref<BTTask> child);
	void remove_child_at_index(int index);
    void clear_children();
    void swap_child(godot::Ref<BTTask> old_child, godot::Ref<BTTask> new_child);

	_FORCE_INLINE_ bool has_child(const godot::Ref<BTTask> child) const
    {
        return (this->children.find(child) != -1);
    }
    _FORCE_INLINE_ int get_child_index(const godot::Ref<BTTask> child) const
    {
        return this->children.find(child);
    }
    bool has_running_child() const;

    BTTask::Status execute(double delta);
    void abort();

    virtual godot::Ref<BTTask> copy() const;
    virtual godot::Ref<BTTask> clone() const;

    void initialize(godot::Node* actor, godot::Ref<Blackboard> blackboard);

    virtual void set_default_name();

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(BTTask::Status);

#define TASK_FAIL() /**********************************************************************************************************************************************************/\
    return BTTask::Status::FAILURE

#define TASK_RUNNING() /*******************************************************************************************************************************************************/\
    return BTTask::Status::RUNNING

#define TASK_SUCCEED() /*******************************************************************************************************************************************************/\
    return BTTask::Status::SUCCESS

#define TASK_FAIL_COND(condition) /********************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_RUNNING_COND(condition) /*****************************************************************************************************************************************/\
    if (condition)                                                                                                                                                              \
    {                                                                                                                                                                           \
        return BTTask::Status::RUNNING;                                                                                                                                         \
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
        godot::UtilityFunctions::printerr(this->get_custom_name(), ": ", message);                                                                                              \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_COMPLAIN_COND(condition, message) /*******************************************************************************************************************************/\
    if ((condition) && (this->is_complain_enabled()))                                                                                                                           \
    {                                                                                                                                                                           \
        godot::UtilityFunctions::printerr(this->get_custom_name(), ": ", message);                                                                                              \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_FAIL_COND_COMP(condition) /***************************************************************************************************************************************/\
    TASK_COMPLAIN_COND(condition, #condition);                                                                                                                                  \
    TASK_FAIL_COND(condition)

#define TASK_FAIL_COND_COMP_MSG(condition, message) /**************************************************************************************************************************/\
    TASK_COMPLAIN_COND(condition, message);                                                                                                                                     \
    TASK_FAIL_COND(condition)


#endif // BT_TASK_HPP