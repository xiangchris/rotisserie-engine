#ifndef LUA_API_H
#define LUA_API_H

#include "Helper.h"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include <thread>

class LuaAPI
{
public:
    static lua_State* GetLuaState() { return L; }

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
    static inline lua_State* L = nullptr;

    static void InitState();
    static void InitAPI();
};

#endif