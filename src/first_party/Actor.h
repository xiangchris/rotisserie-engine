#ifndef ACTOR_H
#define ACTOR_H

#include "CollisionDetector.h"
#include "ComponentManager.h"

#include "rapidjson/document.h"

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <unordered_map>

class Collision;

class Actor
{
public:
	Actor(const rapidjson::Value& doc);
	Actor(const Actor& other);

	void Start();
	void Update();
	void LateUpdate();

	void EditorUpdate();

	void CollisionEnter(Collision& collision);
	void CollisionExit(Collision& collision);
	void TriggerEnter(Collision& collision);
	void TriggerExit(Collision& collision);

	void Destroy();

	void ProcessAddedComponents();
	void ProcessRemovedComponents();

	// Identifier getters
	std::string& GetName() { return actor_name; }
	int GetID() const { return actor_id; }

	// Component getters
	luabridge::LuaRef GetComponentByKey(const std::string& key);
	luabridge::LuaRef GetComponent(const std::string& type_name);
	luabridge::LuaRef GetComponents(const std::string& type_name);

	luabridge::LuaRef AddComponent(const std::string& type_name);
	void RemoveComponent(luabridge::LuaRef component_ref);

private:
	// Creation methods
	Actor& operator=(const Actor& template_actor);
	void GetTemplateValues(const rapidjson::Value& doc);

	// Identifiers
	static inline int next_id = 0;
	int actor_id;
	std::string actor_name = "";

	// Component storage
	std::map<std::string, Component> actor_components;
	std::unordered_map<std::string, std::set<std::string>> type_to_component_key;
	std::vector<Component*> sprite_renderer_components;

	// Deletion markers
	bool alive = true;
	bool retain = false;

	// Component creation and deletion
	static inline int next_component = 0;
	std::deque<Component> components_to_add;
	std::vector<Component*> components_to_remove;

	friend class SceneDB;
	friend class EditorManager;
};

#endif
