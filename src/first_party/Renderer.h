#ifndef RENDERER_H
#define RENDERER_H

#include "Actor.h"

#include "glm/glm.hpp"
#include "Helper.h"

#include <string>

class Renderer
{
private:
    static inline const float PIXELS_PER_METER = 100.0f;
    struct ImageDrawRequest
    {
        SDL_Texture* texture = nullptr;
        SDL_FRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
        int rotation = 0;
        float scale_x = 1.0f;
        float scale_y = 1.0f;
        float pivot_x = 0.5f;
        float pivot_y = 0.5f;
        int r = 255;
        int g = 255;
        int b = 255;
        int a = 255;
        int sorting_order = 0;
    };
    struct CompareImageRequests {
        bool operator()(const ImageDrawRequest& lhs, const ImageDrawRequest& rhs) const {
            return lhs.sorting_order < rhs.sorting_order;
        }
    };

    struct UIDrawRequest
    {
        SDL_Texture* texture = nullptr;
        SDL_FRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
        int r = 255;
        int g = 255;
        int b = 255;
        int a = 255;
        int sorting_order = 0;
    };
    struct CompareUIRequests {
        bool operator()(const UIDrawRequest& lhs, const UIDrawRequest& rhs) const {
            return lhs.sorting_order < rhs.sorting_order;
        }
    };

    struct TextDrawRequest
    {
        SDL_Texture* texture;
        SDL_FRect rect;
    };

    struct PixelDrawRequest
    {
        int x = 0; 
        int y = 0;
        int r = 255;
        int g = 255;
        int b = 255;
        int a = 255;
    };

public:
    static void Init(const std::string& game_title);

    static void RenderClear();
    static void Present();
    
    static void MoveCamera(float x, float y) { camera_position = glm::vec2(x, y); }
    static float GetCameraX() { return camera_position.x; }
    static float GetCameraY() { return camera_position.y; }
    static void SetZoom(float zoom);
    static float GetZoom() { return zoom_factor; }

    // Image Drawing
    static ImageDrawRequest & CreateImageRequest(const std::string& image_name, float x, float y);
    static void Draw(const std::string& image_name, float x, float y);
    static void DrawEx(const std::string& image_name, float x, float y, float rotation_degrees,
        float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);
    static void DrawImage(const std::string& image_name, float x, float y, float rotation_degrees, 
        float scale_x, float scale_y, float pivot_x, float pivot_y, int r, int g, int b, int a, int sorting_order);

    // UI Drawing
    static UIDrawRequest& CreateUIRequest(const std::string& image_name, int x, int y);
    static void DrawUI(const std::string& image_name, float x, float y);
    static void DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, float sorting_order);

    // Text Drawing
    static void DrawText(const std::string& text_content, float x, float y, const std::string& font_name, float font_size, float r, float g, float b, float a);

    // Pixel Drawing
    static void DrawPixel(float x, float y, float r, float g, float b, float a);

    static inline SDL_Window* GetWindow() { return window; };
    static inline SDL_Renderer* GetRenderer() { return renderer; };
    static const glm::vec2& GetWindowSize() { return window_size; };


private:
    static inline SDL_Window* window = nullptr;
    static inline SDL_Renderer* renderer = nullptr;

    static inline glm::vec2 window_size = glm::vec2(640.0f, 360.0f);
    static inline glm::vec2 window_center = glm::vec2(320.f, 180.0f);

    static inline float zoom_factor = 1.0f;
    static inline float inverse_zoom = 1.0f;
    static inline glm::vec2 window_box = glm::vec2(800.0f, 450.0f);

    static inline glm::vec2 camera_position = glm::vec2(0.0f, 0.0f);
    static inline int clear_color_r = 255;
    static inline int clear_color_g = 255;
    static inline int clear_color_b = 255;

    static void RenderAndClearImageDrawRequests();
    static void RenderAndClearUIDrawRequests();
    static void RenderAndClearTextDrawRequests();
    static void RenderAndClearPixelDrawRequests();

    static inline std::vector<ImageDrawRequest> image_draw_request_queue;
    static inline std::vector<UIDrawRequest> ui_draw_request_queue;
    static inline std::vector<TextDrawRequest> text_draw_request_queue;
    static inline std::vector<PixelDrawRequest> pixel_draw_request_queue;
};

#endif
