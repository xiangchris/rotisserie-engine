#ifndef IMAGE_DB_H
#define IMAGE_DB_H

#include "Helper.h"
#include "glm/glm.hpp"

#include <string>
#include <unordered_map>


struct Image
{
    SDL_Texture* texture = nullptr;
    glm::vec2 image_size = glm::vec2(0.0f, 0.0f);
};


class ImageDB
{
public:
    static void Init();
    static Image& GetImage(const std::string& image_name);

    static void CreateDefaultParticleTextureWithName(const std::string& name);

private:
    static inline std::unordered_map<std::string, Image> loaded_images;
};

#endif
