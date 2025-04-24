#include "ComponentManager.h"

#include "ComponentDB.h"

#include "Rigidbody.h"
#include "ParticleSystem.h"

#include "LuaAPI.h"

// Create a new component of component_type
void ComponentManager::CreateComponent(luabridge::LuaRef& ref, const std::string& component_type)
{
    if (ComponentDB::IsComponentTypeNative(component_type))
    {
        if (component_type == "Rigidbody")
            return CreateNativeComponent<Rigidbody>(ref);
        if (component_type == "ParticleSystem")
            return CreateNativeComponent<ParticleSystem>(ref);
    }
    else
    {
        return EstablishLuaInheritance(ref, *ComponentDB::GetLuaComponent(component_type));
    }
}

void ComponentManager::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table, const std::string& component_type)
{
    if (ComponentDB::IsComponentTypeNative(component_type))
    {
        if (component_type == "Rigidbody")
            return EstablishNativeInheritance<Rigidbody>(instance_table, parent_table);
        if (component_type == "ParticleSystem")
            return EstablishNativeInheritance<ParticleSystem>(instance_table, parent_table);
    }
    else
    {
        return EstablishLuaInheritance(instance_table, parent_table);
    }
}

// Establish Inheratiance
void ComponentManager::EstablishLuaInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table)
{
    // Get the lua state
    lua_State* L = LuaAPI::GetLuaState();

    // We must create a metatable to establish inheritance in lua.
    luabridge::LuaRef new_metatable = luabridge::newTable(L);
    new_metatable["__index"] = parent_table;

    // We must use the raw lua C-API (lua stack) to perform a "setmetatable" operation.
    instance_table.push(L);
    new_metatable.push(L);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
}

template <typename T>
void ComponentManager::CreateNativeComponent(luabridge::LuaRef& ref)
{
    T* obj = new T();
    ref = luabridge::LuaRef(LuaAPI::GetLuaState(), obj);
}

template<typename T>
inline void ComponentManager::EstablishNativeInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table)
{
    T* obj_parent = parent_table.cast<T*>();
    instance_table = luabridge::LuaRef(LuaAPI::GetLuaState(), new T(*obj_parent));
}

std::vector<std::string> ComponentManager::GetKeyVector(const std::string& component_type) {
    std::vector<std::string> result;
    
    if (ComponentDB::IsComponentTypeNative(component_type))
    {

    }
    else
    {
        if (component_type_to_keys.find(component_type) != component_type_to_keys.end())
            return component_type_to_keys[component_type];
        else
        {
            for (luabridge::Iterator it(*ComponentDB::GetLuaComponent(component_type)); !it.isNil(); ++it)
                result.push_back(it.key().tostring());
            component_type_to_keys[component_type] = result;
        }
    }

    return result;
}

// Component constructor
Component::Component()
{
    component_ref = std::make_shared<luabridge::LuaRef>(luabridge::newTable(LuaAPI::GetLuaState()));
    (*component_ref)["enabled"] = true;
    
    has_start = false;
    has_update = false;
    has_late_update = false;

    has_collision_enter = false;
    has_collision_exit = false;
    has_trigger_enter = false;
    has_trigger_exit = false;

    has_destroy = false;

    alive = true;
}

Component& Component::operator=(const Component& parent_component)
{
    luabridge::LuaRef& ref = *component_ref;
    type = parent_component.type;
    // Establish inheritance to base component
    ComponentManager::EstablishLuaInheritance(ref, *parent_component.component_ref);

    has_start = parent_component.has_start;
    has_update = parent_component.has_update;
    has_late_update = parent_component.has_late_update;

    has_collision_enter = parent_component.has_collision_enter;
    has_collision_exit = parent_component.has_collision_exit;
    has_trigger_enter = parent_component.has_trigger_enter;
    has_trigger_exit = parent_component.has_trigger_exit;

    has_destroy = parent_component.has_destroy;

    return *this;
}

// Return true if component is enabled
bool Component::IsEnabled() const
{
    return (*component_ref)["enabled"];
}


