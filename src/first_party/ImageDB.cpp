#include "ImageDB.h"

#include "Renderer.h"

#include <filesystem>


// Initialize SDL_image, pull all .png files in folder and add to database
void ImageDB::Init()
{
    IMG_Init(IMG_INIT_PNG);
    const std::string directory_path = "resources/images";
    if (std::filesystem::exists(directory_path))
    {
        for (const auto& file : std::filesystem::directory_iterator(directory_path))
        {
            SDL_Texture* new_texture = IMG_LoadTexture(Renderer::GetRenderer(), file.path().string().c_str());
            Image& new_image = loaded_images[file.path().stem().string()];
            new_image.texture = new_texture;
            Helper::SDL_QueryTexture(new_texture, &new_image.image_size.x, &new_image.image_size.y);
        }
    }
}

// Given string image_name, return Image struct. Return error if not in database
Image& ImageDB::GetImage(const std::string& image_name)
{
    auto it = loaded_images.find(image_name);
    if (it == loaded_images.end())
    {
        std::cout << "error: missing image " << image_name;
        exit(0);
    }
    else
    {
        return it->second;
    }
}

void ImageDB::CreateDefaultParticleTextureWithName(const std::string& name)
{
    if (loaded_images.find(name) != loaded_images.end())
        return;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);

    Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    SDL_FillRect(surface, NULL, white_color);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer::GetRenderer(), surface);

    SDL_FreeSurface(surface);

    Image& new_image = loaded_images[name];
    new_image.texture = texture;
    Helper::SDL_QueryTexture(texture, &new_image.image_size.x, &new_image.image_size.y);
}
