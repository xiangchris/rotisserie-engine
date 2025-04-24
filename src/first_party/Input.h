#ifndef INPUT_H
#define INPUT_H

#include "SDL2/SDL.h"
#include "glm/glm.hpp"

#include <unordered_map>
#include <vector>
#include <string>

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

class Input
{
public:
	static void Init(); // Call before main loop begins.
	static void ProcessEvent(const SDL_Event& e); // Call every frame at start of event loop.
	static void LateUpdate(); // Call at very end of frame.
	
	static bool GetKey(const SDL_Scancode& keycode); // Is a key down currently?
	static bool GetKeyDown(const SDL_Scancode& keycode); // Was a key pressed this frame?
	static bool GetKeyUp(const SDL_Scancode& keycode); // Was a key released this frame?

	static glm::vec2 GetMousePosition();

	static bool GetMouseButton(int button);
	static bool GetMouseButtonDown(int button);
	static bool GetMouseButtonUp(int button);
	static float GetMouseScrollDelta();

	static void HideCursor() { SDL_ShowCursor(SDL_DISABLE); }
	static void ShowCursor() { SDL_ShowCursor(SDL_ENABLE); }

private:
	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static inline std::vector<SDL_Scancode> just_became_down_scancodes;
	static inline std::vector<SDL_Scancode> just_became_up_scancodes;

	static inline glm::vec2 mouse_position;
	static inline std::unordered_map<int, INPUT_STATE> mouse_button_states;
	static inline std::vector<int> just_became_down_buttons;
	static inline std::vector<int> just_became_up_buttons;

	static inline float mouse_scroll_this_frame = 0;
};

#endif
