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

	static const std::vector<std::string>& GetNativeKeys(const std::string& component_type);
private:
	static inline std::unordered_map<std::string, std::unique_ptr<luabridge::LuaRef>> loaded_components;
	static inline std::vector<std::string> components_types;
	
	static inline const std::vector<std::string> native_types = {
	"Rigidbody",
	"ParticleSystem",
	//"SpriteRenderer"
	};

	static inline const std::unordered_set<std::string> native_types_set = {
	"Rigidbody",
	"ParticleSystem",
	//"SpriteRenderer"
	};

	static inline const std::vector<std::string> rigidbody = {
		"x", "y", "body_type", "precise", "gravity_scale",
		"density", "angular_friction", "rotation", "has_collider",
		"collider_type", "width", "height", "radius", "friction", "bounciness",
		"has_trigger", "trigger_type", "trigger_width", "trigger_height", "trigger_radius"
	};
	static inline const std::vector<std::string> particle_system = {
		"x", "y", "frames_between_bursts", "burst_quantity", "start_scale_min",
		"start_scale_max", "rotation_min", "rotation_max", "start_color_r",
		"start_color_g", "start_color_b", "start_color_a", "emit_radius_min",
		"emit_radius_max", "emit_angle_min", "emit_angle_max", "image",
		"sorting_order", "duration_frames", "start_speed_min", "start_speed_max",
		"rotation_speed_min", "rotation_speed_max", "gravity_scale_x",
		"gravity_scale_y", "drag_factor", "angular_drag_factor", "end_scale",
		"end_color_r", "end_color_g", "end_color_b", "end_color_a"
	};
};

#endif
