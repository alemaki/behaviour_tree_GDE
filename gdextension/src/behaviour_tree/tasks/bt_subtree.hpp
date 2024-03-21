#ifndef BT_SUBTREE_HPP
#define BT_SUBTREE_HPP

#include "behaviour_tree/behaviour_tree.hpp"

#include <godot_cpp/classes/packed_scene.hpp>

class BTSubtree : public BTTask
{
    GDCLASS(BTSubtree, BTTask);

private:
    godot::String file_path;

public:
    void set_file_path(const godot::String& p_file_path);
    _FORCE_INLINE_ godot::String get_file_path() const
    {
        return file_path;
    }
    BehaviourTree* load_behavior_tree() const;

    virtual godot::Ref<BTTask> clone() const override;

protected:
    static void _bind_methods();

};


#endif // BT_SUBTREE_HPP