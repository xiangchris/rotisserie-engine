#ifndef EVENTBUS_H
#define EVENTBUS_H

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include <deque>
#include <memory>
#include <unordered_map>

class Subscription
{
public:
    Subscription(const std::string& event, luabridge::LuaRef& component, luabridge::LuaRef& foo);

    bool operator==(const Subscription& other);

    std::string event_type;
    std::shared_ptr<luabridge::LuaRef> component_ref;
    std::shared_ptr<luabridge::LuaRef> function;
};

class EventBus
{
public:
    static void Publish(std::string event_type, luabridge::LuaRef event_object);
    static void Subscribe(std::string event_type, luabridge::LuaRef component_ref, luabridge::LuaRef function);
    static void Unsubscribe(std::string event_type, luabridge::LuaRef component_ref, luabridge::LuaRef function);

    static void ProcessSubscriptions();

private:
    // event_type -> component_ref, function
    static inline std::unordered_map<std::string, std::deque<Subscription>> topics;

    static inline std::deque<Subscription> subscriptions_to_add;
    static inline std::deque<Subscription> subscriptions_to_remove;
};

#endif
