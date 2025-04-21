#include "SceneDB.h"

#include "EngineUtils.h"
#include "TemplateDB.h"


// Load all actors in scene
void SceneDB::LoadScene(const std::string scene_name)
{
    std::string path = "resources/scenes/" + scene_name + ".scene";
    if (!std::filesystem::exists(path))
    {
        std::cout << "error: scene " << scene_name << " is missing";
        exit(0);
    }
    UnloadScene();
    current_scene = scene_name;
    LoadActors(path);
}

// Clear containers associated with current scene, reload retained actors that are alive
void SceneDB::UnloadScene()
{
    static std::vector<Actor*> retained_actors;
    retained_actors.clear();
    retained_actors.reserve(loaded_actors.size());

    name_to_actor.clear();
    next_scene.clear();


    for (Actor* a : loaded_actors)
    {
        if (!a->alive) continue;

        if (a->retain)
        {
            retained_actors.push_back(a);
            name_to_actor[a->actor_name].insert(a);
        }
        else
            a->Destroy();
    }

    loaded_actors = std::move(retained_actors);
}

void SceneDB::RestartScene()
{
    loaded_actors.clear();
}

// Populate loaded_actor vector
void SceneDB::LoadActors(const std::string& path)
{
    rapidjson::Document doc;
    EngineUtils::ReadJsonFile(path, doc);
    const rapidjson::Value& actors = doc["actors"];

    // foreach actor a in actors, set values and emplace back
    for (const rapidjson::Value& a : actors.GetArray())
    {
        Actor* new_actor = new_actors_to_add.emplace_back(new Actor(a));
        name_to_actor[new_actor->actor_name].insert(new_actor);
    }
}

// Call lifetime functions on all actors
void SceneDB::UpdateActors()
{
    std::vector<Actor*> alive_actors;
    alive_actors.reserve(loaded_actors.size() + actors_to_add.size());

    // Add alive actors to vector
    for (Actor* a : loaded_actors)
        if (a->alive)
            alive_actors.push_back(a);

    actors_to_add.clear();
    std::swap(actors_to_add, new_actors_to_add);
    // Add new actors into alive actors
    for (Actor* a : actors_to_add)
        if (a->alive)
        {
            a->Start();
            alive_actors.push_back(a);
        }

    // Add new components on alive actors
    for (Actor* a : alive_actors)
        if (a->alive)
            a->ProcessAddedComponents();

#ifndef NDEBUG
    if (stopped)
    {
        // Update function on alive actors
        for (Actor* a : alive_actors)
            if (a->alive)
                a->EditorUpdate();
    }
    else
#endif
    {
        // Update function on alive actors
        for (Actor* a : alive_actors)
            if (a->alive)
                a->Update();

        // Late Update function on alive actors
        for (Actor* a : alive_actors)
            if (a->alive)
                a->LateUpdate();
    }


    // Add new components on alive actors
    for (Actor* a : alive_actors)
        if (a->alive)
            a->ProcessRemovedComponents();

    // Clean all previously killed actors
    loaded_actors = std::move(alive_actors);
}

// Find actor from name
luabridge::LuaRef SceneDB::Find(const std::string& name)
{
    auto it = name_to_actor.find(name);
    if (it != name_to_actor.end() && !it->second.empty())
        return luabridge::LuaRef(lua_state, *it->second.begin());

    return luabridge::LuaRef(lua_state);
}

// Find all actors with name
luabridge::LuaRef SceneDB::FindAll(const std::string& name)
{
    luabridge::LuaRef table = luabridge::newTable(lua_state);
    int i = 1;

    auto it = name_to_actor.find(name);
    if (it != name_to_actor.end())
        for (Actor* a : it->second)
            table[i++] = a;

    return table;
}

// Create new Actor and add to queue
luabridge::LuaRef SceneDB::Instantiate(const std::string& actor_template_name)
{
    Actor* new_actor = new_actors_to_add.emplace_back(new Actor(*TemplateDB::GetTemplate(actor_template_name)));
    name_to_actor[new_actor->actor_name].insert(new_actor);
    return luabridge::LuaRef(lua_state, new_actor);
}

// Mark actor for deletion, erase from name database
void SceneDB::Destroy(Actor* victim)
{
    victim->Destroy();
    name_to_actor[victim->actor_name].erase(victim);
}

void SceneDB::QueueLoad(const std::string& scene_name)
{
    next_scene = scene_name;
}

std::string& SceneDB::GetCurrent()
{
    return current_scene;
}

void SceneDB::DontDestroy(Actor* actor)
{
    actor->retain = true;
}
