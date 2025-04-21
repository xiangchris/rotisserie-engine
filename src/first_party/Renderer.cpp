#include "Renderer.h"

#include "EngineUtils.h"
#include "ImageDB.h"
#include "TextDB.h"
#include "EditorManager.h"

#include "SDL_ttf.h"

#include <algorithm>

void Renderer::Init(const std::string& game_title)
{
    rapidjson::Document doc;
    if (std::filesystem::exists("resources/rendering.config"))
    {
        EngineUtils::ReadJsonFile("resources/rendering.config", doc);
        EngineUtils::GetConfigValue(doc, "x_resolution", window_size.x);
        EngineUtils::GetConfigValue(doc, "y_resolution", window_size.y);
        EngineUtils::GetConfigValue(doc, "clear_color_r", clear_color_r);
        EngineUtils::GetConfigValue(doc, "clear_color_g", clear_color_g);
        EngineUtils::GetConfigValue(doc, "clear_color_b", clear_color_b);
    }
    window_center = window_size * 0.5f * inverse_zoom;
    window_box = window_size * inverse_zoom * 1.1f;

    window = Helper::SDL_CreateWindow(game_title.c_str(), 0, 30, static_cast<int>(window_size.x), static_cast<int>(window_size.y), SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
}

void Renderer::RenderClear()
{
    SDL_SetRenderDrawColor(renderer, clear_color_r, clear_color_g, clear_color_b, 255);
    SDL_RenderClear(renderer);
}

void Renderer::Present()
{
    RenderAndClearImageDrawRequests();
    RenderAndClearUIDrawRequests();
    RenderAndClearTextDrawRequests();
    RenderAndClearPixelDrawRequests();

#ifndef NDEBUG
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), Renderer::GetRenderer());
#endif

    Helper::SDL_RenderPresent(renderer);
}

void Renderer::SetZoom(float zoom)
{
    zoom_factor = zoom;
    inverse_zoom = 1.0f / zoom_factor;
    window_center = window_size * 0.5f * inverse_zoom;
    window_box = window_size * inverse_zoom * 1.1f;
}

/***************************************************************
                          Image Drawing
 ***************************************************************/

Renderer::ImageDrawRequest& Renderer::CreateImageRequest(const std::string& image_name, float x, float y)
{
    ImageDrawRequest& request = image_draw_request_queue.emplace_back();
    Image& image = ImageDB::GetImage(image_name);
    request.texture = image.texture;

    request.rect = {
        x,
        y,
        image.image_size.x,
        image.image_size.y
    };
    return request;
}

void Renderer::Draw(const std::string& image_name, float x, float y)
{
    CreateImageRequest(image_name, x, y);
}

void Renderer::DrawEx(const std::string& image_name, float x, float y, float rotation_degrees,
    float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order)
{
    DrawImage(image_name, x, y, rotation_degrees, scale_x, scale_y, pivot_x, pivot_y,
        static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), static_cast<int>(sorting_order));
}

void Renderer::DrawImage(const std::string& image_name, float x, float y, float rotation_degrees,
    float scale_x, float scale_y, float pivot_x, float pivot_y, int r, int g, int b, int a, int sorting_order)
{
    ImageDrawRequest& request = CreateImageRequest(image_name, x, y);

    request.rotation = static_cast<int>(rotation_degrees);
    request.scale_x = scale_x;
    request.scale_y = scale_y;
    request.pivot_x = pivot_x;
    request.pivot_y = pivot_y;
    request.r = r;
    request.g = g;
    request.b = b;
    request.a = a;
    request.sorting_order = sorting_order;
}

void Renderer::RenderAndClearImageDrawRequests()
{
    std::stable_sort(image_draw_request_queue.begin(), image_draw_request_queue.end(), CompareImageRequests());
    SDL_RenderSetScale(renderer, zoom_factor, zoom_factor);

    for (auto& request : image_draw_request_queue)
    {
        request.rect.w *= glm::abs(request.scale_x);
        request.rect.h *= glm::abs(request.scale_y);

        SDL_FPoint pivot_point = { request.pivot_x * request.rect.w, request.pivot_y * request.rect.h };

        request.rect.x = (request.rect.x - camera_position.x) * PIXELS_PER_METER + window_center.x - pivot_point.x;
        request.rect.y = (request.rect.y - camera_position.y) * PIXELS_PER_METER + window_center.y - pivot_point.y;

        if (request.rect.x + request.rect.w < -50.0f || request.rect.y + request.rect.h < -50.0f || request.rect.x > window_box.x || request.rect.y > window_box.y)
            continue;

        int flip_mode = SDL_FLIP_NONE;
        if (request.scale_x < 0.0f)
            flip_mode |= SDL_FLIP_HORIZONTAL;
        if (request.scale_y < 0.0f)
            flip_mode |= SDL_FLIP_VERTICAL;

        SDL_SetTextureColorMod(request.texture, request.r, request.g, request.b);
        SDL_SetTextureAlphaMod(request.texture, request.a);

        Helper::SDL_RenderCopyEx(
            -1,
            "",
            renderer,
            request.texture,
            NULL,
            &request.rect,
            static_cast<float>(request.rotation),
            &pivot_point,
            static_cast<SDL_RendererFlip>(flip_mode)
        );

        SDL_SetTextureColorMod(request.texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(request.texture, 255);
    }

    SDL_RenderSetScale(renderer, 1.0f, 1.0f);
    image_draw_request_queue.clear();
}

/***************************************************************
                          UI Drawing
 ***************************************************************/

Renderer::UIDrawRequest& Renderer::CreateUIRequest(const std::string& image_name, int x, int y)
{
    UIDrawRequest& request = ui_draw_request_queue.emplace_back();
    Image& image = ImageDB::GetImage(image_name);
    request.texture = image.texture;
    request.rect = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(image.image_size.x),
        static_cast<float>(image.image_size.y)
    };
    return request;
}

void Renderer::DrawUI(const std::string& image_name, float x, float y)
{
    CreateUIRequest(image_name, static_cast<int>(x), static_cast<int>(y));
}

void Renderer::DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, float sorting_order)
{
    UIDrawRequest& request = CreateUIRequest(image_name, static_cast<int>(x), static_cast<int>(y));
    request.r = static_cast<int>(r);
    request.g = static_cast<int>(g);
    request.b = static_cast<int>(b);
    request.a = static_cast<int>(a);
    request.sorting_order = static_cast<int>(sorting_order);
}

void Renderer::RenderAndClearUIDrawRequests()
{
    std::stable_sort(ui_draw_request_queue.begin(), ui_draw_request_queue.end(), CompareUIRequests());

    for (auto& request : ui_draw_request_queue)
    {
        SDL_SetTextureColorMod(request.texture, static_cast<Uint8>(request.r), static_cast<Uint8>(request.g), static_cast<Uint8>(request.b));
        SDL_SetTextureAlphaMod(request.texture, request.a);
        Helper::SDL_RenderCopy(renderer, request.texture, NULL, &request.rect);
        SDL_SetTextureColorMod(request.texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(request.texture, 255);
    }

    ui_draw_request_queue.clear();
}

/***************************************************************
                          Text Drawing
 ***************************************************************/


void Renderer::DrawText(const std::string& text_content, float x, float y, const std::string& font_name, float font_size, float r, float g, float b, float a)
{
    int _x = static_cast<int>(x), _y = static_cast<int>(y);
    int _r = static_cast<int>(r), _g = static_cast<int>(g), _b = static_cast<int>(b), _a = static_cast<int>(a);
    TextDrawRequest& request = text_draw_request_queue.emplace_back();
    
    TTF_Font* font = TextDB::GetFont(font_name, static_cast<int>(font_size));
    int w, h;
    TTF_SizeText(font, text_content.c_str(), &w, &h);

    SDL_Color text_color = { 
        static_cast<Uint8>(_r),
        static_cast<Uint8>(_g),
        static_cast<Uint8>(_b),
        static_cast<Uint8>(_a)
    };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text_content.c_str(), text_color);

    request.texture = SDL_CreateTextureFromSurface(Renderer::GetRenderer(), surface);
    request.rect = {
        static_cast<float>(_x),
        static_cast<float>(_y),
        static_cast<float>(w),
        static_cast<float>(h)
    };

    SDL_FreeSurface(surface);
}

void Renderer::RenderAndClearTextDrawRequests()
{
    for (auto& request : text_draw_request_queue)
    {
        Helper::SDL_RenderCopy(renderer, request.texture, nullptr, &request.rect);
        SDL_DestroyTexture(request.texture);
    }

    text_draw_request_queue.clear();
}

/***************************************************************
                          Pixel Drawing
 ***************************************************************/

void Renderer::DrawPixel(float x, float y, float r, float g, float b, float a)
{
    PixelDrawRequest& request = pixel_draw_request_queue.emplace_back();
    request.x = static_cast<int>(x);
    request.y = static_cast<int>(y);
    request.r = static_cast<int>(r);
    request.g = static_cast<int>(g);
    request.b = static_cast<int>(b);
    request.a = static_cast<int>(a);
}

void Renderer::RenderAndClearPixelDrawRequests()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (auto& request: pixel_draw_request_queue)
    {
        SDL_SetRenderDrawColor(renderer, request.r, request.g, request.b, request.a);
        SDL_RenderDrawPoint(renderer, request.x, request.y);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    pixel_draw_request_queue.clear();
}
