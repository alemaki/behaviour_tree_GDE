#ifndef SIGNAL_WATCHER_HPP
#define SIGNAL_WATCHER_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

class SignalWatcher
{
private:
    static std::unordered_map<std::string, int> signal_count;
    static std::unordered_set<void*> watched_objects;

private:
    static void on_signal_emitted(const godot::String& signal_key)
    {
        std::string signal_key_std = signal_key.utf8().ptr();
        signal_count[signal_key_std]++;
    }

public:
    static void watch_signals(godot::Object* target)
    {
        if (watched_objects.find(target) != watched_objects.end())
        {
            SignalWatcher::reset_object(target);
            return;
        }

        godot::Array signal_list = target->get_signal_list();
        for (int i = 0; i < signal_list.size(); i++) 
        {
            godot::Dictionary signal_info = signal_list[i];
            godot::String signal_name = signal_info["name"];
            godot::String signal_key = godot::itos(target->get_instance_id()) + signal_name;
            
            std::string signal_key_std = signal_key.utf8().ptr();
            signal_count[signal_key_std] = 0;
            watched_objects.insert(target);
            target->connect(signal_name, callable_mp_static(&SignalWatcher::on_signal_emitted).bind(signal_key));
        }
    }

    static bool was_signal_emitted(godot::Object* target, const godot::String& signal_name)
    {
        std::string signal_key = (godot::itos(target->get_instance_id()) + signal_name).utf8().ptr();
        auto it = signal_count.find(signal_key);
        return it != signal_count.end() && it->second;
    }

    static int get_signal_emitted_count(godot::Object* target, const godot::String& signal_name)
    {
        std::string signal_key = (godot::itos(target->get_instance_id()) + signal_name).utf8().ptr();
        auto it = signal_count.find(signal_key);
        return it != signal_count.end() ? it->second : 0;
    }

    static void reset()
    {
        for (auto& [key, value] : signal_count)
        {
            value = 0;
        }
    }

    static void reset_object(godot::Object* target)
    {
        if (watched_objects.find(target) == watched_objects.end())
        {
            return;
        }

        godot::Array signal_list = target->get_signal_list();
        for (int i = 0; i < signal_list.size(); i++) 
        {
            godot::Dictionary signal_info = signal_list[i];
            godot::String signal_name = signal_info["name"];
            std::string signal_key = std::string((char*)(godot::itos(target->get_instance_id()) + signal_name)._native_ptr());

            std::cout << signal_key << std::endl;

            signal_count[signal_key] = 0;
        }
    }
};

#endif /* SIGNAL_WATCHER_HPP */