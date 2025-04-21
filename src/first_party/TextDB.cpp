#include "TextDB.h"

#include "Renderer.h"

#include <filesystem>


void TextDB::Init()
{
    TTF_Init();
}

TTF_Font* TextDB::GetFont(const std::string& font_name, const int font_size)
{
    auto it = font_map[font_name].find(font_size);
    if (it == font_map[font_name].end())
    {
        const std::string font_directory_path = "resources/fonts/" + font_name + ".ttf";

        if (std::filesystem::exists(font_directory_path))
        {
            return font_map[font_name][font_size] = TTF_OpenFont(font_directory_path.c_str(), font_size);
        }
        else
        {
            std::cout << "error: font " + font_name + " missing";
            exit(0);
        }
    }

    return it->second;
}
