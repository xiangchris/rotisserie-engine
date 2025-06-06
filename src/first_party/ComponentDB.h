#ifndef COMPONENT_DB_H
#define COMPONENT_DB_H

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include <memory>

extern inline lua_State* lua_state = nullptr;

class ComponentDB
{
public:
	static void Init();
	static std::vector<std::string>& GetKeys(const std::string& component_type);
	static void GetComponent(luabridge::LuaRef& ref, const std::string& component_type);
	
	template <typename T>
	static void CreateCpp(luabridge::LuaRef& ref)
	{
		T* obj = new T();
		ref = luabridge::LuaRef(lua_state, obj);
	}

	template <typename T>
	static void CreateCpp(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table)
	{
		T* obj_parent = parent_table.cast<T*>();
		instance_table = luabridge::LuaRef(lua_state, new T(*obj_parent));
	}

	static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);
	static std::vector<std::string> ListAllComponentTypes();

	static void ResetDatabase() { loaded_components.clear(); }

private:
	static inline std::unordered_map<std::string, std::unique_ptr<luabridge::LuaRef>> loaded_components;
	static std::vector<std::string>& FindKeys(luabridge::LuaRef ref);
	static std::vector<std::string>& FindKeysCpp(const std::string& component_type);
};

#endif
