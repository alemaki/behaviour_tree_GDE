#include "signal_watcher.hpp"

#ifndef SIGNAL_WATCHER_GLOBALS
#define SIGNAL_WATCHER_GLOBALS
std::unordered_map<std::string, int> SignalWatcher::signal_count;
std::unordered_set<void*> SignalWatcher::watched_objects;
#endif /* SIGNAL_WATCHER_GLOBALS */