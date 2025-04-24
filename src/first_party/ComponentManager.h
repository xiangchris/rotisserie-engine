#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "ComponentDB.h"

#include "Helper.h"

#include <string>
#include <unordered_set>

class ComponentManager
{
public:
    // Create a new component of component_type
    static void CreateComponent(luabridge::LuaRef& ref, const std::string& component_type);

    static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table, const std::string& component_type);
    static void EstablishLuaInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);

    static std::vector<std::string> GetKeyVector(const std::string& component_type);

private:
    template <typename T>
    static void CreateNativeComponent(luabridge::LuaRef& ref);

    template <typename T>
    static void EstablishNativeInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);
};

class Component
{
public:
    explicit Component();
    Component& operator=(const Component& parent_component);

    bool IsEnabled() const;

    std::shared_ptr<luabridge::LuaRef> component_ref;
    std::string type;

    bool has_start;
    bool has_update;
    bool has_late_update;

    bool has_collision_enter;
    bool has_collision_exit;
    bool has_trigger_enter;
    bool has_trigger_exit;

    bool has_destroy;

    bool alive;
};

#endif
