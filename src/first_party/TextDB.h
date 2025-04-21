#ifndef TEXT_DB_H
#define TEXT_DB_H

#include "SDL_ttf/SDL_ttf.h"

#include <string>
#include <unordered_map>


class TextDB
{
public:
    static void Init();
    static TTF_Font* GetFont(const std::string& font_name, const int font_size);

private:
    static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> font_map;
};

#endif
