#include "signal_watcher.hpp"

std::unordered_map<std::string, int> SignalWatcher::signal_count;
std::unordered_map<std::string, godot::Array> SignalWatcher::signal_last_arguments;
std::unordered_set<void*> SignalWatcher::watched_objects;

void SignalWatcher::on_signal_emitted_0_params(const godot::String& signal_key)
{
    std::string signal_key_std = signal_key.utf8().ptr();
    signal_count[signal_key_std]++;
    signal_last_arguments[signal_key_std] = godot::Array();
}

void SignalWatcher::on_signal_emitted_1_params(godot::Variant param1, const godot::String& signal_key)
{
    std::string signal_key_std = signal_key.utf8().ptr();
    signal_count[signal_key_std]++;

    godot::Array arr;
    arr.resize(1);
    arr[0] = param1;
    signal_last_arguments[signal_key_std] = arr;
}

void SignalWatcher::on_signal_emitted_2_params(godot::Variant param1, godot::Variant param2, const godot::String& signal_key)
{
    std::string signal_key_std = signal_key.utf8().ptr();
    signal_count[signal_key_std]++;

    godot::Array arr;
    arr.resize(2);
    arr[0] = param1;
    arr[1] = param2;
    signal_last_arguments[signal_key_std] = arr;
}

void SignalWatcher::on_signal_emitted_3_params(godot::Variant param1, godot::Variant param2, godot::Variant param3, const godot::String& signal_key)
{
    std::string signal_key_std = signal_key.utf8().ptr();
    signal_count[signal_key_std]++;
    
    godot::Array arr;
    arr.resize(3);
    arr[0] = param1;
    arr[1] = param2;
    arr[2] = param3;
    signal_last_arguments[signal_key_std] = arr;
}

void SignalWatcher::on_signal_emitted_4_params(godot::Variant param1, godot::Variant param2, godot::Variant param3, godot::Variant param4, const godot::String& signal_key)
{
    std::string signal_key_std = signal_key.utf8().ptr();
    signal_count[signal_key_std]++;
    
    godot::Array arr;
    arr.resize(4);
    arr[0] = param1;
    arr[1] = param2;
    arr[2] = param3;
    arr[3] = param4;
    signal_last_arguments[signal_key_std] = arr;
}

void SignalWatcher::on_signal_emitted_5_params(godot::Variant param1, godot::Variant param2, godot::Variant param3, godot::Variant param4, godot::Variant param5, const godot::String& signal_key)
{
    std::string signal_key_std = signal_key.utf8().ptr();
    signal_count[signal_key_std]++;
    
    godot::Array arr;
    arr.resize(5);
    arr[0] = param1;
    arr[1] = param2;
    arr[2] = param3;
    arr[3] = param4;
    arr[4] = param5;
    signal_last_arguments[signal_key_std] = arr;
}

void SignalWatcher::connect_target_signal(godot::Object* target, const godot::Dictionary& signal_info)
{
    godot::String signal_name = signal_info["name"];
    godot::String signal_key = godot::itos(target->get_instance_id()) + signal_name;
    std::string signal_key_std = signal_key.utf8().ptr();

    signal_count[signal_key_std] = 0;

    watched_objects.insert(target);

    int arg_size = (godot::Array(signal_info["args"])).size();

    godot::Error result = godot::Error::OK;
    if (arg_size == 0)
    {
        result = target->connect(signal_name, callable_mp_static(&SignalWatcher::on_signal_emitted_0_params).bind(signal_key));
    }
    if (arg_size == 1)
    {
        result = target->connect(signal_name, callable_mp_static(&SignalWatcher::on_signal_emitted_1_params).bind(signal_key));
    }
    if (arg_size == 2)
    {
        result = target->connect(signal_name, callable_mp_static(&SignalWatcher::on_signal_emitted_2_params).bind(signal_key));
    }
    if (arg_size == 3)
    {
        result = target->connect(signal_name, callable_mp_static(&SignalWatcher::on_signal_emitted_3_params).bind(signal_key));
    }
    if (arg_size == 4)
    {
        result = target->connect(signal_name, callable_mp_static(&SignalWatcher::on_signal_emitted_4_params).bind(signal_key));
    }
    if (arg_size == 5)
    {
        result = target->connect(signal_name, callable_mp_static(&SignalWatcher::on_signal_emitted_5_params).bind(signal_key));
    }

    if (result != godot::OK) 
    {
        godot::UtilityFunctions::print("Failed to connect signal: ", signal_name, " on object: ", target);
    }
    else
    {
        //godot::UtilityFunctions::print("Successfully connected signal: ", signal_name, " on object: ", target);
    }
}

void SignalWatcher::watch_signals(godot::Object* target)
{
    SignalWatcher::reset_object(target);

    godot::Array signal_list = target->get_signal_list();
    for (int i = 0; i < signal_list.size(); i++) 
    {
        godot::Dictionary signal_info = signal_list[i];
        SignalWatcher::connect_target_signal(target, signal_info);
    }
}

bool SignalWatcher::signal_emitted(godot::Object* target, const godot::String& signal_name)
{
    std::string signal_key = (godot::itos(target->get_instance_id()) + signal_name).utf8().ptr();
    auto it = signal_count.find(signal_key);
    return (it != signal_count.end()) && (it->second != 0);
}

int SignalWatcher::get_signal_emitted_count(godot::Object* target, const godot::String& signal_name)
{
    std::string signal_key = (godot::itos(target->get_instance_id()) + signal_name).utf8().ptr();
    auto it = signal_count.find(signal_key);
    return it != signal_count.end() ? it->second : 0;
}

godot::Array SignalWatcher::get_signal_emitted_arguments(godot::Object* target, const godot::String& signal_name)
{
    if (!(SignalWatcher::signal_emitted(target, signal_name)))
    {
        return godot::Array();
    }

    std::string signal_key = (godot::itos(target->get_instance_id()) + signal_name).utf8().ptr();
    return signal_last_arguments[signal_key];
}

void SignalWatcher::reset()
{
    for (auto& [key, value] : signal_count)
    {
        value = 0;
    }
}

void SignalWatcher::reset_object(godot::Object* target)
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
        std::string signal_key = (godot::itos(target->get_instance_id()) + signal_name).utf8().ptr();

        signal_count[signal_key] = 0;
    }
}
