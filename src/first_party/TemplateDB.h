#ifndef TEMPLATE_DB_H
#define TEMPLATE_DB_H

#include "Actor.h"

#include <memory>
#include <unordered_map>


class TemplateDB
{
public:
    static void LoadAll();
    static Actor* GetTemplate(const std::string& template_name);
    static std::vector<std::string> ListAllTemplateTypes();
private:
    static void CreateTemplate(const std::string& template_name);
    static inline std::unordered_map<std::string, std::unique_ptr<Actor>> template_map;
};

#endif
