#ifndef COMPONENT_DB_H
#define COMPONENT_DB_H

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include <string>
#include <memory>
#include <unordered_set>
#include <vector>

class ComponentDB
{
public:
	static void Init();
	static luabridge::LuaRef* GetLuaComponent(const std::string& component_type);

	static bool IsComponentTypeNative(std::string type);
	static std::vector<std::string> GetAllComponentTypes();

private:
	static inline std::unordered_map<std::string, std::unique_ptr<luabridge::LuaRef>> loaded_components;
	static inline std::vector<std::string> components_types;
	
	static inline const std::vector<std::string> native_types = {
	"Rigidbody",
	"ParticleSystem",
	"SpriteRenderer"
	};

	static inline const std::unordered_set<std::string> native_types_set = {
	"Rigidbody",
	"ParticleSystem",
	"SpriteRenderer"
	};
};

#endif
