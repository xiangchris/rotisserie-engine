#ifndef SCENE_DB_H
#define SCENE_DB_H

#include "Actor.h"
#include "EditorManager.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>


class SceneDB
{
public:
    static void LoadScene(const std::string scene_name);

    static void RestartScene();

    static void UpdateActors();

    static luabridge::LuaRef Find(const std::string& name);
    static luabridge::LuaRef FindAll(const std::string& name);

    static luabridge::LuaRef Instantiate(const std::string& actor_template_name);
    static void Destroy(Actor* victim);

    static void QueueLoad(const std::string& scene_name);
    static std::string& GetCurrent();
    static void DontDestroy(Actor* actor);
    static inline std::string next_scene;

private:
    static inline std::vector<Actor*> loaded_actors;

    struct CompareActors {
        bool operator()(const Actor* lhs, const Actor* rhs) const {
            return lhs->actor_id < rhs->actor_id;  // If sizes are equal, compare lexicographically
        }
    };
    static inline std::unordered_map<std::string, std::set<Actor*, CompareActors>> name_to_actor;

    static void LoadActors(const std::string& path);
    static void UnloadScene();

    static inline int next_component = 1;
    static inline std::deque<Actor*> actors_to_add;
    static inline std::deque<Actor*> new_actors_to_add;

    static inline std::string current_scene;
    static inline bool stopped = true;

    friend class EditorManager;
};

#endif
