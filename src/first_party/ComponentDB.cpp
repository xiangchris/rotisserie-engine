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

std::unordered_map<std::string, luabridge::LuaRef> ComponentDB::GetKeyValueMap(luabridge::LuaRef& ref) {
    std::unordered_map<std::string, luabridge::LuaRef> result;
    if (ref.isNil()) { return result; }

    auto L = ref.state();
    push(L, ref); // push table

    // Check if table has a metatable with __index
    if (lua_getmetatable(L, -1)) {  // pushes metatable
        lua_pushstring(L, "__index"); // push "__index"
        lua_rawget(L, -2);            // get metatable.__index, pops "__index"

        if (lua_istable(L, -1)) {
            // We found a metatable with __index table, recursively get its properties
            luabridge::LuaRef indexTable = luabridge::LuaRef::fromStack(L, -1);

            // Recursively get properties from the __index table
            auto inheritedProps = GetKeyValueMap(indexTable);
            result.insert(inheritedProps.begin(), inheritedProps.end());
        }

        lua_pop(L, 2); // pop metatable.__index and metatable
    }

    // Now get direct properties (will override any inherited ones with the same name)
    push(L, ref); // make sure table is at top of stack


    lua_pushnil(L);  // push nil, so lua_next removes it from stack and puts (k, v) on stack
    while (lua_next(L, -2) != 0) // -2, because we have table at -1
    {
        if (lua_isstring(L, -2)) // only store stuff with string keys
            result.emplace(lua_tostring(L, -2), luabridge::LuaRef::fromStack(L, -1));
        lua_pop(L, 1); // remove value, keep key for lua_next
    }

    lua_pop(L, 1); // pop table
    return result;
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
