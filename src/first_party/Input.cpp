#include "Input.h"

void Input::Init()
{
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++)
    {
        keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
    }
}

// Take event type and put it into maps and vectors
void Input::ProcessEvent(const SDL_Event& e)
{
    switch (e.type)
    {
    case SDL_KEYDOWN:
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_scancodes.push_back(e.key.keysym.scancode);
        break;
    case SDL_KEYUP:
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(e.key.keysym.scancode);
        break;
    case SDL_MOUSEMOTION:
        mouse_position = glm::vec2(e.button.x, e.button.y);
        break;
    case SDL_MOUSEBUTTONDOWN:
        mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_buttons.push_back(e.button.button);
        break;
    case SDL_MOUSEBUTTONUP:
        mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_buttons.push_back(e.button.button);
        break;
    case SDL_MOUSEWHEEL:
        mouse_scroll_this_frame = e.wheel.preciseY;
        break;
    default:
        break;
    }
}

void Input::LateUpdate()
{
    for (const SDL_Scancode& code : just_became_down_scancodes)
    {
        keyboard_states[code] = INPUT_STATE_DOWN;
    }
    just_became_down_scancodes.clear();

    for (const SDL_Scancode& code : just_became_up_scancodes)
    {
        keyboard_states[code] = INPUT_STATE_UP;
    }
    just_became_up_scancodes.clear();

    for (const int button : just_became_down_buttons)
    {
        mouse_button_states[button] = INPUT_STATE_DOWN;
    }
    just_became_down_buttons.clear();

    for (const int button : just_became_up_buttons)
    {
        mouse_button_states[button] = INPUT_STATE_UP;
    }
    just_became_up_buttons.clear();

    mouse_scroll_this_frame = 0.0f;
}

bool Input::GetKey(const SDL_Scancode& keycode)
{
    return keyboard_states[keycode] == INPUT_STATE_DOWN || keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(const SDL_Scancode& keycode)
{
    return keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(const SDL_Scancode& keycode)
{
    return keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_UP;
}

glm::vec2 Input::GetMousePosition()
{
    return mouse_position;
}

bool Input::GetMouseButton(int button)
{
    return mouse_button_states[button] == INPUT_STATE_DOWN || mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonDown(int button)
{
    return mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonUp(int button)
{
    return mouse_button_states[button] == INPUT_STATE_JUST_BECAME_UP;
}

float Input::GetMouseScrollDelta()
{
    return mouse_scroll_this_frame;
}
