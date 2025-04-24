#include "Actor.h"

#include "AudioDB.h"
#include "EngineUtils.h"
#include "ImageDB.h"
#include "LuaAPI.h"
#include "Renderer.h"
#include "SceneDB.h"
#include "TemplateDB.h"

#include <algorithm>

// Constructor for Actor, if this is a template, pull from rapidjson value
Actor::Actor(const rapidjson::Value& doc)
{
    // Get template if specified
    GetTemplateValues(doc);

    // Get all actor values
    actor_id = next_id++;
    EngineUtils::GetConfigValue(doc, "name", actor_name);

    // Find components, return if not found
    rapidjson::Value::ConstMemberIterator i = doc.FindMember("components");
    if (i == doc.MemberEnd()) { return; }

    // Iterate through all components
    const rapidjson::Value& components = i->value;
    for (rapidjson::Value::ConstMemberIterator it = components.MemberBegin(); it != components.MemberEnd(); it++)
    {
        // Get key and create new_component in actor component map
        std::string key = it->name.GetString();
        bool is_new = actor_components.find(key) == actor_components.end();
        Component& component = actor_components[key];        

        // Luaref reference
        luabridge::LuaRef& ref = *component.component_ref;

        // Establish inheritance to base component if new, if component is from template we already have inheritance
        if (is_new)
        {
            std::string type = it->value["type"].GetString();

            ComponentManager::CreateComponent(ref, type);

#ifndef NDEBUG
            if (type == "SpriteRenderer")
                sprite_renderer_components.push_back(&component);
#endif

            ref["key"] = key;
            component.type = type;

            type_to_component_key[type].insert(key);

            component.has_start = ref["OnStart"].isFunction();
            component.has_update = ref["OnUpdate"].isFunction();
            component.has_late_update = ref["OnLateUpdate"].isFunction();

            component.has_collision_enter = ref["OnCollisionEnter"].isFunction();
            component.has_collision_exit = ref["OnCollisionExit"].isFunction();
            component.has_trigger_enter = ref["OnTriggerEnter"].isFunction();
            component.has_trigger_exit = ref["OnTriggerExit"].isFunction();

            component.has_destroy = ref["OnDestroy"].isFunction();
        }

        // Set actor attribute in component to actor pointer
        ref["actor"] = this;

        // Iterate through all overrides, insert it into lua table
        const rapidjson::Value& values = it->value;
        for (rapidjson::Value::ConstMemberIterator jt = values.MemberBegin(); jt != values.MemberEnd(); jt++)
            EngineUtils::GetComponentOverride(ref, jt->name.GetString(), jt->value);
    }
}

// Copy constructor
Actor::Actor(const Actor& other)
{
    *this = other;
    actor_id = next_id++;
}

// Get template if it is specified
void Actor::GetTemplateValues(const rapidjson::Value& doc)
{
    // Templates do not have a template property
    std::string template_name = "";
    EngineUtils::GetConfigValue(doc, "template", template_name);

    if (!template_name.empty())
        *this = *TemplateDB::GetTemplate(template_name);
}

// Actor equality operator, copy all primitives and make new copies of components and establish inheratiance
Actor& Actor::operator=(const Actor& other)
{
    // Avoid self-assignment.
    if (this != &other)
    {
        actor_name = other.actor_name;

        // Iterate through all components and copies to self.
        for (const auto& [key, parent_component] : other.actor_components)
        {
            // Get key and create new_component in actor component map
            Component& new_component = actor_components[key];

            new_component = parent_component;

#ifndef NDEBUG
            if (new_component.type == "SpriteRenderer")
                sprite_renderer_components.push_back(&new_component);
#endif

            // create a reference to component_ref
            type_to_component_key[new_component.type].insert(key);

            luabridge::LuaRef& ref = *new_component.component_ref;

            // Set actor attribute in component to actor pointer
            ref["actor"] = this;
            ref["key"] = key;
        }
    }
    return *this;
}

void Actor::Start()
{
    for (auto& [key, component] : actor_components)
    {
        if (!component.has_start || !component.alive || !component.IsEnabled())
            continue;

        component.has_start = false;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnStart"](ref);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::Update()
{
    for (auto& [key, component] : actor_components)
    {
        if (!component.has_update || !component.alive || !component.IsEnabled())
            continue;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnUpdate"](ref);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::LateUpdate()
{
    for (auto& [key, component] : actor_components)
    {
        if (!component.has_late_update || !component.alive || !component.IsEnabled())
            continue;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnLateUpdate"](ref);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::EditorUpdate()
{
    for (auto& component : sprite_renderer_components)
    {
        if (!component->has_update || !component->alive || !component->IsEnabled())
            continue;

        try
        {
            luabridge::LuaRef& ref = *component->component_ref;
            ref["OnUpdate"](ref);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::CollisionEnter(Collision& collision)
{
    for (auto& [key, component] : actor_components)
    {
        if (!component.has_collision_enter || !component.alive || !component.IsEnabled())
            continue;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnCollisionEnter"](ref, collision);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::CollisionExit(Collision& collision)
{
    for (auto& [key, component] : actor_components)
    {
        if (!component.has_collision_exit || !component.alive || !component.IsEnabled())
            continue;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnCollisionExit"](ref, collision);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::TriggerEnter(Collision& collision)
{
    for (auto& [key, component] : actor_components)
    {
        if (!component.has_trigger_enter || !component.alive || !component.IsEnabled())
            continue;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnTriggerEnter"](ref, collision);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::TriggerExit(Collision& collision)
{
    for (auto& [key, component] : actor_components)
    {
        if (!component.has_trigger_exit || !component.alive || !component.IsEnabled())
            continue;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnTriggerExit"](ref, collision);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::Destroy()
{
    alive = false;
    for (auto& [key, component] : actor_components)
    {
        if (component.alive)
            component.alive = false;
        else
            continue;
        if (!component.has_destroy)
            continue;

        try
        {
            luabridge::LuaRef& ref = *component.component_ref;
            ref["OnDestroy"](ref);
        }
        catch (luabridge::LuaException& e)
        {
            EngineUtils::ReportError(actor_name, e);
        }
    }
}

void Actor::ProcessAddedComponents()
{
    for (Component& c : components_to_add)
    {
        if (!c.alive)
            continue;
        
        luabridge::LuaRef& ref = *c.component_ref;
        if (c.has_start && c.IsEnabled())
        {
            c.has_start = false;
        
            try
            {
                ref["OnStart"](ref);
            }
            catch (luabridge::LuaException& e)
            {
                EngineUtils::ReportError(actor_name, e);
            }
        }
        actor_components[ref["key"]] = std::move(c);
    }
    components_to_add.clear();
}

// Do OnDestroy for all removed components
void Actor::ProcessRemovedComponents()
{
    for (Component* c : components_to_remove)
    {
        luabridge::LuaRef& ref = *c->component_ref;
        if (c->has_destroy)
        {
            try
            {
                ref["OnDestroy"](ref);
            }
            catch (luabridge::LuaException& e)
            {
                EngineUtils::ReportError(actor_name, e);
            }
        }
    }
    components_to_remove.clear();
}

// Return LuaRef to actor component from component key
luabridge::LuaRef Actor::GetComponentByKey(const std::string& key)
{
    auto it = actor_components.find(key);
    if (it == actor_components.end() || !it->second.alive)
        return luabridge::LuaRef(LuaAPI::GetLuaState());
    return *it->second.component_ref;
}

// Return LuaRef to first actor component from type name
luabridge::LuaRef Actor::GetComponent(const std::string& type_name)
{
    // Dead components will not be in type_to_component
    auto it = type_to_component_key.find(type_name);
    if (it != type_to_component_key.end() && !it->second.empty())
        return *actor_components[*it->second.begin()].component_ref;
    return luabridge::LuaRef(LuaAPI::GetLuaState());
}

// Return LuaRef to table of all actor components of type name
luabridge::LuaRef Actor::GetComponents(const std::string& type_name)
{
    luabridge::LuaRef table = luabridge::newTable(LuaAPI::GetLuaState());
    int i = 1;

    auto it = type_to_component_key.find(type_name);
    if (it != type_to_component_key.end())
        for (const std::string& key : it->second)
            table[i++] = *actor_components[key].component_ref;

    return table;
}

// Queue component for initialization
luabridge::LuaRef Actor::AddComponent(const std::string& type_name)
{
    // Get key and create new_component in actor component map
    std::string key = "r" + std::to_string(next_component++);
    
    while (actor_components.find(key) != actor_components.end())
        key = "r" + std::to_string(next_component++);

    Component& component = components_to_add.emplace_back();

    // create a reference to component_ref
    luabridge::LuaRef& ref = *component.component_ref;
    
    // Get component_ref
    ComponentManager::CreateComponent(ref, type_name);

    component.has_start = ref["OnStart"].isFunction();
    component.has_update = ref["OnUpdate"].isFunction();
    component.has_late_update = ref["OnLateUpdate"].isFunction();

    component.has_collision_enter = ref["OnCollisionEnter"].isFunction();
    component.has_collision_exit = ref["OnCollisionExit"].isFunction();
    component.has_trigger_enter = ref["OnTriggerEnter"].isFunction();
    component.has_trigger_exit = ref["OnTriggerExit"].isFunction();

    component.has_destroy = ref["OnDestroy"].isFunction();

    // Set actor attribute in component to actor pointer
    ref["actor"] = this;
    ref["key"] = key;
    component.type = type_name;
    type_to_component_key[type_name].insert(key);

#ifndef NDEBUG
    if (type_name == "SpriteRenderer")
        sprite_renderer_components.push_back(&component);
#endif

    return *component.component_ref;
}

// Mark component for deletion
void Actor::RemoveComponent(luabridge::LuaRef victim_ref)
{
    // Get key and reference of victim
    std::string key = victim_ref["key"];
    Component& victim = actor_components[key];

    // Mark component as not dead and disable component
    victim.alive = false;
    
    // Remove victim from type_vector 
    type_to_component_key[victim.type].erase(key);
    components_to_remove.push_back(&victim);
}
