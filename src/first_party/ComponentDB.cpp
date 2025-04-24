#include "ComponentDB.h"

#include "LuaAPI.h"

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
            if (luaL_dofile(LuaAPI::GetLuaState(), file.path().string().c_str()) != LUA_OK)
            {
                std::cout << "problem with lua file " << component_name;
                exit(0);
            }
            
            // Get unique_ptr to LuaState, add it to map
            loaded_components.insert(
                {
                    component_name,
                    std::make_unique<luabridge::LuaRef>(luabridge::getGlobal(LuaAPI::GetLuaState(), component_name.c_str()))
                });
#ifndef NDEBUG
            components_types.push_back(component_name);
#endif
        }
    }
}

luabridge::LuaRef* ComponentDB::GetLuaComponent(const std::string& component_type)
{
    auto it = loaded_components.find(component_type);
    if (it == loaded_components.end())
    {
        std::cout << "error: failed to locate component " << component_type;
        exit(0);
    }

    return it->second.get();
}

std::vector<std::string> ComponentDB::GetAllComponentTypes()
{
    static std::vector<std::string> list;
    
    if (list.size() != components_types.size() + native_types.size())
    {
        list.clear();
        list = components_types;
        list.insert(list.end(), native_types.begin(), native_types.end());
    }
    
    return list;
}


bool ComponentDB::IsComponentTypeNative(std::string type)
{
    return native_types_set.find(type) != native_types_set.end();
}
