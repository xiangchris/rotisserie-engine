#include "EventBus.h"

void EventBus::Publish(std::string event_type, luabridge::LuaRef event_object)
{
    auto it = topics.find(event_type);
    if (it == topics.end())
        return;

    auto& subscriber_list = it->second;
    for (auto& sub : subscriber_list)
    {
        (*sub.function)(*sub.component_ref, event_object);
    }
}

void EventBus::Subscribe(std::string event_type, luabridge::LuaRef component_ref, luabridge::LuaRef function)
{
    subscriptions_to_add.emplace_back(event_type, component_ref, function);
}

void EventBus::Unsubscribe(std::string event_type, luabridge::LuaRef component_ref, luabridge::LuaRef function)
{
    subscriptions_to_remove.emplace_back(event_type, component_ref, function);
}

void EventBus::ProcessSubscriptions()
{
    for (auto& sub : subscriptions_to_add)
    {
        topics[sub.event_type].push_back(std::move(sub));
    }

    for (auto& sub : subscriptions_to_remove)
    {
        auto& subscriber_list = topics[sub.event_type];
        auto it = std::find(subscriber_list.begin(), subscriber_list.end(), sub);

        if (it != subscriber_list.end())
            subscriber_list.erase(it);
    }

    subscriptions_to_add.clear();
    subscriptions_to_remove.clear();
}

Subscription::Subscription(const std::string& event, luabridge::LuaRef& component, luabridge::LuaRef& foo)
{
    event_type = event;
    component_ref = std::make_shared<luabridge::LuaRef>(component);
    function = std::make_shared<luabridge::LuaRef>(foo);
}

bool Subscription::operator==(const Subscription& other)
{
    return *function == *other.function && *component_ref == *other.component_ref && event_type == other.event_type;
}
