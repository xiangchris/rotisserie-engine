#pragma once

#include "Actor.h"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "rapidjson/document.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class EditorManager
{
protected:
    EditorManager() { }

    static inline EditorManager* editor_;

public:
    static EditorManager* Get();

    void Init();
    
    void ShowEditor();

private:
    void GetScenes();

    void ShowSceneHierarchy();
    void ActorNameBox(Actor* actor);
    void ActorSaveToTemplateButton(Actor* actor);

    void HeaderAddActor();
    void ShowComponent(luabridge::LuaRef& ref, const std::string& type);

    void ShowPlayPauseStepButtons();

    void ShowSceneMenu();
    void NewSceneButton();
    void CreateScene(const std::string& scene_name);
    void DisplayScenes();
    void SaveScene();
    void AddSceneActorsToDocument(rapidjson::Document& doc);
    void WriteJsonFile(const std::string& path, rapidjson::Document& doc);

    void ActorToJson(const Actor* a, rapidjson::Value& a_json, rapidjson::Document::AllocatorType& allocator);


    std::vector<std::string> scenes;

    bool play_mode = false;
    bool paused = false;
    std::string start_scene;
};
