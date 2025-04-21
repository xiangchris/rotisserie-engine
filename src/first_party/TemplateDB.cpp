#include "TemplateDB.h"

#include "EngineUtils.h"

void TemplateDB::LoadAll()
{
    template_map.clear();
    const std::string directory_path = "resources/actor_templates";
    if (std::filesystem::exists(directory_path))
    {
        // Check lua scripts and add them to the global state as tables
        for (const auto& file : std::filesystem::directory_iterator(directory_path))
        {
            CreateTemplate(file.path().stem().string());
        }
    }
}

Actor* TemplateDB::GetTemplate(const std::string& template_name)
{
    if (template_map.find(template_name) == template_map.end())
        CreateTemplate(template_name);

    return template_map[template_name].get();
}

std::vector<std::string> TemplateDB::ListAllTemplateTypes()
{
    static std::vector<std::string> list;
    list.clear();
    list.reserve(template_map.size());
    
    for (auto& pair : template_map)
    {
        list.push_back(pair.first);
    }
    return list;
}


void TemplateDB::CreateTemplate(const std::string& template_name)
{
    std::string path = "resources/actor_templates/" + template_name + ".template";
    if (!std::filesystem::exists(path))
    {
        std::cout << "error: template " << template_name << " is missing";
        exit(0);
    }

    rapidjson::Document doc;
    EngineUtils::ReadJsonFile(path, doc);

    std::unique_ptr<Actor> new_template = std::make_unique<Actor>(doc);
    template_map[template_name] = std::move(new_template);
}