#ifndef ENGINE_H
#define ENGINE_H

#include "rapidjson/document.h"

class Engine
{
public:
    void GameLoop();

private:
    void LoadInitialSettings();
    void CheckForFiles();
    void RendererInit(rapidjson::Document& doc);

    void ProcessInput();

    bool game_running = true;
};

#endif
