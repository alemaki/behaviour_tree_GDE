#ifndef SIGNAL_WATCHER_HPP
#define SIGNAL_WATCHER_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

class SignalWatcher
{
private:
    static std::unordered_map<std::string, int> signal_count;
    static std::unordered_map<std::string, godot::Array> signal_last_arguments;
    static std::unordered_set<void*> watched_objects;

private:
    /* In Godot, signals can have a maximum of 5 parameters. 
    This limitation is built into the Godot engine.*/
    static void on_signal_emitted_0_params(const godot::String& signal_key);
    static void on_signal_emitted_1_params(godot::Variant param1,
                                           const godot::String& signal_key);
    static void on_signal_emitted_2_params(godot::Variant param1,
                                           godot::Variant param2,
                                           const godot::String& signal_key);
    static void on_signal_emitted_3_params(godot::Variant param1,
                                           godot::Variant param2,
                                           godot::Variant param3,
                                           const godot::String& signal_key);
    static void on_signal_emitted_4_params(godot::Variant param1,
                                           godot::Variant param2,
                                           godot::Variant param3,
                                           godot::Variant param4,
                                           const godot::String& signal_key);
    static void on_signal_emitted_5_params(godot::Variant param1,
                                           godot::Variant param2,
                                           godot::Variant param3,
                                           godot::Variant param4,
                                           godot::Variant param5,
                                           const godot::String& signal_key);
    
    static void connect_target_signal(godot::Object* target, const godot::Dictionary& signal_info);

public:
    static void watch_signals(godot::Object* target);
    static bool signal_emitted(godot::Object* target, const godot::String& signal_name);
    static int get_signal_emitted_count(godot::Object* target, const godot::String& signal_name);
    static godot::Array get_signal_emitted_arguments(godot::Object* target, const godot::String& signal_name);
    static void reset();
    static void reset_object(godot::Object* target);
};

#endif /* SIGNAL_WATCHER_HPP */
