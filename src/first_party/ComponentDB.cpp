#include "ComponentDB.h"

#include "Rigidbody.h"
#include "ParticleSystem.h"

#include <filesystem>

// Add all lua files in folder to database
void ComponentDB::Init()
{
    const std::string directory_path = "resources/component_types";
    if (std::filesystem::exists(directory_path))
    {
        // Check lua scripts and add them to the global state as tables
        for (const auto& file : std::filesystem::directory_iterator(directory_path))
        {
            std::string component_name = file.path().stem().string();
            if (luaL_dofile(lua_state, file.path().string().c_str()) != LUA_OK)
            {
                std::cout << "problem with lua file " << component_name;
                exit(0);
            }
            
            // Get unique_ptr to LuaState, add it to map
            loaded_components.insert(
                {
                    component_name,
                    std::make_unique<luabridge::LuaRef>(luabridge::getGlobal(lua_state, component_name.c_str()))
                });
        }
    }
}

std::vector<std::string>& ComponentDB::GetKeys(const std::string& component_type)
{
    auto it = loaded_components.find(component_type);
    if (it != loaded_components.end())
    {
        return FindKeys(*it->second.get());
    }
    else
    {
        return FindKeysCpp(component_type);
    }
}

std::vector<std::string>& ComponentDB::FindKeys(luabridge::LuaRef ref)
{
    static std::vector<std::string> keys;
    keys.clear();

    if (!ref.isTable())
        return keys;

    for (luabridge::Iterator it(ref); !it.isNil(); ++it)
    {
        keys.push_back(it.key().cast<std::string>());
    }

    std::sort(keys.begin(), keys.end());

    return keys;
}

std::vector<std::string>& ComponentDB::FindKeysCpp(const std::string& component_type)
{
    static std::vector<std::string> rigidbody = {
    "x", "y", "body_type", "precise", "gravity_scale", 
    "density", "angular_friction", "rotation", "has_collider",
    "collider_type", "width", "height", "radius", "friction", "bounciness",
    "has_trigger", "trigger_type", "trigger_width", "trigger_height", "trigger_radius"
    };
    static std::vector<std::string> particle_system = {
    "x", "y", "frames_between_bursts", "burst_quantity", "start_scale_min",
    "start_scale_max", "rotation_min", "rotation_max", "start_color_r",
    "start_color_g", "start_color_b", "start_color_a", "emit_radius_min",
    "emit_radius_max", "emit_angle_min", "emit_angle_max", "image",
    "sorting_order", "duration_frames", "start_speed_min", "start_speed_max",
    "rotation_speed_min", "rotation_speed_max", "gravity_scale_x",
    "gravity_scale_y", "drag_factor", "angular_drag_factor", "end_scale",
    "end_color_r", "end_color_g", "end_color_b", "end_color_a"
    };

    if (component_type == "Rigidbody")
        return rigidbody;
    else
        return particle_system;
}

// Create a new component of component_type
void ComponentDB::GetComponent(luabridge::LuaRef& ref, const std::string& component_type)
{
    auto it = loaded_components.find(component_type);
    if (it == loaded_components.end())
    {
        if (component_type == "Rigidbody") { return CreateCpp<Rigidbody>(ref); }
        if (component_type == "ParticleSystem") { return CreateCpp<ParticleSystem>(ref); }

        std::cout << "error: failed to locate component " << component_type;
        exit(0);
    }

    return EstablishInheritance(ref, *it->second.get());
}

// Establish Inheratiance
void ComponentDB::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table)
{
    // We must create a metatable to establish inheritance in lua.
    luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
    new_metatable["__index"] = parent_table;

    // We must use the raw lua C-API (lua stack) to perform a "setmetatable" operation.
    instance_table.push(lua_state);
    new_metatable.push(lua_state);
    lua_setmetatable(lua_state, -2);
    lua_pop(lua_state, 1);
}

std::vector<std::string> ComponentDB::ListAllComponentTypes()
{
    static std::vector<std::string> list;
    list.clear();
    list.reserve(loaded_components.size());

    for (auto& pair : loaded_components)
    {
        list.push_back(pair.first);
    }
    list.push_back("Rigidbody");
    list.push_back("ParticleSystem");
    return list;
}
