#ifndef BT_TASK_HPP
#define BT_TASK_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>

#include "blackboard/blackboard.hpp"

#include "behaviour_tree/utils/macros.hpp"

/**
 * @class BTTask
 * @brief Base class for Behaviour Tree tasks in the Demonless AI system.
 *
 * A BTTask represents a single node (or “task”) in a behaviour tree.
 * Tasks define logic for AI agents to perform actions or make decisions.
 * This class provides a foundation for all of the task types,
 * handling execution flow, parent-child relationships, and lifecycle management.
 *
 * Each task follows a simple lifecycle of virtuals that can be overriden:
 *   - `_setup()`  → Called once before the task is first used.
 *   - `_enter()`  → Called when the task begins execution.
 *   - `_tick()`   → Called repeatedly while the task runs; returns a Status.
 *   - `_exit()`   → Called when the task finishes or is aborted.
 *
 * Tasks can maintain children (for composites like selectors or sequences),
 * and a shared blackboard for communication between them.
 */
class BTTask : public godot::Resource
{
    GDCLASS(BTTask, godot::Resource);

public:
    /**
     * @brief Represents the current execution state of the task.
     */
    enum Status
    {
        FRESH,   ///< Task has not started yet.
        RUNNING, ///< Task is currently running.
        SUCCESS, ///< Task completed successfully.
        FAILURE, ///< Task failed to complete.
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
    /**
     * @brief Called once when the task is initialized.
     *
     * Use this to set up internal state, allocate resources, or link references.
     */
    virtual void _setup();

    /**
     * @brief Called when the task starts execution.
     *
     * Use this for initialization specific to the beginning of an execution cycle.
     */
    virtual void _enter();

    /**
     * @brief Called when the task stops executing, either through `_tick` - success or failure, or `abort`.
     *
     * Use this to clean up or reset temporary states.
     */
    virtual void _exit();

    /**
     * @brief Called every frame while the task is running.
     *
     * Override this in derived classes to define the task's actual logic.
     *
     * @param delta Frame time delta.
     * @return The current execution status (RUNNING, SUCCESS, or FAILURE).
     */
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

    /**
     * @brief Executes this task.
     *
     * This handles setup, ticking, and exit calls automatically.
     *
     * @param delta Frame time delta.
     * @return The final task status after execution.
     */
    BTTask::Status execute(double delta);

    /**
     * @brief Aborts this task’s current execution.
     *
     * Forces a stop and calls `_exit()`.
     */
    void abort();

    /**
     * @brief Creates a shallow copy of this task and its data with no children.
     * @return A reference to the copied BTTask.
     */
    virtual godot::Ref<BTTask> copy() const;

    /**
     * @brief Clones this task, including its configuration and children.
     * @return A cloned task reference.
     */
    virtual godot::Ref<BTTask> clone() const;

    /**
     * @brief Initializes this task with an actor and a shared blackboard.
     * @param actor The actor node executing this task.
     * @param blackboard Shared data store for the behaviour tree.
     */
    void initialize(godot::Node *actor, godot::Ref<Blackboard> blackboard);

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
        ::godot::_err_print_error(FUNCTION_STR, __FILE__, __LINE__, "Method/function failed.", this->get_custom_name() + godot::String(": ") + message);                        \
        return BTTask::Status::FAILURE;                                                                                                                                         \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_COMPLAIN_COND(condition, message) /*******************************************************************************************************************************/\
    if ((condition) && (this->is_complain_enabled()))                                                                                                                           \
    {                                                                                                                                                                           \
        ::godot::_err_print_error(FUNCTION_STR, __FILE__, __LINE__, "Method/function failed.", this->get_custom_name() + godot::String(": ") + message);                        \
    }                                                                                                                                                                           \
    else ((void)0)

#define TASK_FAIL_COND_COMP(condition) /***************************************************************************************************************************************/\
    TASK_COMPLAIN_COND(condition, #condition);                                                                                                                                  \
    TASK_FAIL_COND(condition)

#define TASK_FAIL_COND_COMP_MSG(condition, message) /**************************************************************************************************************************/\
    TASK_COMPLAIN_COND(condition, message);                                                                                                                                     \
    TASK_FAIL_COND(condition)


#endif // BT_TASK_HPP