#include "Engine.h"

#include "AudioDB.h"
#include "EditorManager.h"
#include "EngineUtils.h"
#include "EventBus.h"
#include "ImageDB.h"
#include "Input.h"
#include "LuaAPI.h"
#include "SceneDB.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "TextDB.h"
#include "TemplateDB.h"

#include "Helper.h"
#include "SDL_ttf/SDL_ttf.h"


// Game loop for engine
void Engine::GameLoop()
{
    LoadInitialSettings();

    while (game_running)
    {
        ProcessInput();

#ifndef NDEBUG
        EditorManager::Get()->ShowEditor();
#endif

        Renderer::RenderClear();
        
        SceneDB::UpdateActors();

        EventBus::ProcessSubscriptions();
        
        Rigidbody::PhysicsStep();

        Renderer::Present();
        
        if (!SceneDB::next_scene.empty())
            SceneDB::LoadScene(SceneDB::next_scene);
    }

#ifndef NDEBUG
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif

    SDL_DestroyRenderer(Renderer::GetRenderer());
    SDL_DestroyWindow(Renderer::GetWindow());
    SDL_Quit();
}

// Initialize actors and camera according to resources files
void Engine::LoadInitialSettings()
{
    CheckForFiles();

    // Read game.config json
    rapidjson::Document doc;
    EngineUtils::ReadJsonFile("resources/game.config", doc);

    RendererInit(doc);
    ImageDB::Init();
    AudioDB::Init();
    TextDB::Init();
    LuaAPI::Init();

#ifndef NDEBUG
    TemplateDB::LoadAll();
    EditorManager::Get()->Init();
#endif
        
    // Pull initial scene from json
    if (doc.HasMember("initial_scene"))
    {
        std::string initial_scene = doc["initial_scene"].GetString();
        SceneDB::LoadScene(initial_scene);
    }
    else
    {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }
}

// Check if resources files exist
void Engine::CheckForFiles()
{
    // Check if resources folder exists
    if (!std::filesystem::exists("resources"))
    {
        std::cout << "error: resources/ missing";
        exit(0);
    }
    // Check if game.config files exists
    if (!std::filesystem::exists("resources/game.config"))
    {
        std::cout << "error: resources/game.config missing";
        exit(0);
    }
}

// Get game title, initalize renderer with game_title
void Engine::RendererInit(rapidjson::Document& doc)
{
    std::string game_title = "";
    EngineUtils::GetConfigValue(doc, "game_title", game_title);
    Renderer::Init(game_title);
}

// Prompt user for input then process it
void Engine::ProcessInput()
{
    Input::LateUpdate();

    SDL_Event e;
    while (Helper::SDL_PollEvent(&e))
    {
#ifndef NDEBUG
        ImGui_ImplSDL2_ProcessEvent(&e);
#endif
        if (e.type == SDL_QUIT)
        {
            game_running = false;
        }
#ifndef NDEBUG
        else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            game_running = false;
        }
#endif
        else
        {
            Input::ProcessEvent(e);
        }
    }
}
