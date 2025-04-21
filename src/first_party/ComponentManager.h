#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "ComponentDB.h"

#include "Helper.h"

#include <thread>

class ComponentManager
{
public:
    static void Init();

    static void Print(const std::string& message) { std::cout << message << std::endl; }

    static void Quit() { exit(0); }
    static void Sleep(const int dur_ms) { std::this_thread::sleep_for(std::chrono::milliseconds(dur_ms)); }
    static int GetFrame() { return Helper::GetFrameNumber(); }
    static void OpenURL(const std::string& url);

    static bool GetKey(const std::string& keycode);
    static bool GetKeyDown(const std::string& keycode);
    static bool GetKeyUp(const std::string& keycode);

private:
    static void InitState();
    static void InitFunctions();
};

class Component
{
public:
    explicit Component();
    Component& operator=(const Component& parent_component);

    bool IsEnabled() const;

    std::shared_ptr<luabridge::LuaRef> component_ref;
    std::string type;

    bool has_start;
    bool has_update;
    bool has_late_update;

    bool has_collision_enter;
    bool has_collision_exit;
    bool has_trigger_enter;
    bool has_trigger_exit;

    bool has_destroy;

    bool alive;
};

#endif