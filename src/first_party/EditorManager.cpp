#include "EditorManager.h"

#include "ComponentDB.h"
#include "Renderer.h"
#include "SceneDB.h"
#include "TemplateDB.h"

#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

// Return singleton pointer to editor manager
EditorManager* EditorManager::Get()
{
    if (editor_ == nullptr)
        editor_ = new EditorManager();
    return editor_;
}

// Initialize ImGui
void EditorManager::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(Renderer::GetWindow(), Renderer::GetRenderer());
    ImGui_ImplSDLRenderer2_Init(Renderer::GetRenderer());

    // Edtior style bc i like this color :)
    ImGui::StyleColorsClassic();

    // Get all scenes in folder
    GetScenes();
}

// Run on init to get all scene names
void EditorManager::GetScenes()
{
    const std::string directory_path = "resources/scenes";
    if (std::filesystem::exists(directory_path))
        // Grab all scene names and push them to the vector
        for (const auto& file : std::filesystem::directory_iterator(directory_path))
            scenes.push_back(file.path().stem().string());
}

// Create ImGui frame and render to window
void EditorManager::ShowEditor()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Windows to show
    ShowSceneHierarchy();
    ShowSceneMenu();
    ShowPlayPauseStepButtons();

    // Render the ImGui frame
    ImGui::Render();
}

// Scene hierarchy window
void EditorManager::ShowSceneHierarchy()
{
    ImGui::Begin("Scene Hierarchy");
    
    // Add new actor menu
    HeaderAddActor();

    int id = 0;
    for (Actor* actor : SceneDB::loaded_actors)
    {
        ImGui::PushID(id++);
        // Header for each actor
        if (ImGui::CollapsingHeader(actor->actor_name.c_str()))
        {
            ActorNameBox(actor);

            // Delete actor button
            if (ImGui::Button("Delete"))
                SceneDB::Destroy(actor);

            ImGui::SameLine();

            // Save actor as template button
            ActorSaveToTemplateButton(actor);

            // Inputs for all components
            for (const auto& pair : actor->actor_components)
            {
                const Component& c = pair.second;

                if (!c.alive)
                    continue;

                // Push a unique  ID for each component
                ImGui::PushID(c.component_ref.get());

                // Enabled checkbox
                bool enabled = (*c.component_ref)["enabled"];
                if (ImGui::Checkbox("##enabled", &enabled))
                    (*c.component_ref)["enabled"] = enabled;

                ImGui::SameLine();
                
                // Expandable node for each component
                if (ImGui::TreeNode(c.type.c_str()))
                {
                    ImGui::Text("key:");
                    ImGui::SameLine();

                    std::string label = "###";
                    std::string key = (*c.component_ref)["key"].tostring();
                    char* str = &key[0];

                    ImGui::InputText(label.c_str(), str, 50);

                    if (ImGui::IsItemDeactivatedAfterEdit())
                        if (!std::string(str).empty())
                            (*c.component_ref)["key"] = std::string(str);

                    if (ImGui::Button("Delete"))
                        actor->RemoveComponent(*c.component_ref);

                    ShowComponent(*c.component_ref, c.type);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            // Add component menu
            if (ImGui::BeginMenu("Add Component"))
            {
                std::vector<std::string> list = ComponentDB::ListAllComponentTypes();

                std::sort(list.begin(), list.end());

                for (std::string name : list)
                {
                    if (ImGui::MenuItem(name.c_str()))
                    {
                        actor->AddComponent(name);
                    }
                }
                ImGui::EndMenu();
            }
        }
        ImGui::PopID();
    }

    ImGui::End();
}

void EditorManager::ActorNameBox(Actor* actor)
{
    ImGui::Text("name:");
    ImGui::SameLine();

    std::string label = "###" + actor->actor_name;
    std::string name = actor->actor_name;
    char* str = &name[0];

    ImGui::InputText(label.c_str(), str, 50);

    if (ImGui::IsItemDeactivatedAfterEdit())
        actor->actor_name = std::string(str);
}

void EditorManager::ActorSaveToTemplateButton(Actor* actor)
{
    static bool show_new_window = false;

    if (ImGui::Button("Save As Template"))
    {
        // Start json document
        rapidjson::Document doc;
        doc.SetObject();
        std::string path = "resources/actor_templates/" + actor->actor_name + ".template";

        // Grab allocator from document
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

        ActorToJson(actor, doc, allocator);

        WriteJsonFile(path, doc);

        TemplateDB::GetTemplate(actor->actor_name);

        show_new_window = true;
    }

    if (show_new_window)
    {
        ImGui::Begin("Confirmation", &show_new_window, ImGuiWindowFlags_NoTitleBar);
       
        ImGui::Text("Actor template ");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        ImGui::SameLine();

        ImGui::Text(actor->actor_name.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine();
        
        ImGui::Text(" created.");

        if (ImGui::Button("Close"))
            show_new_window = false;

        ImGui::End();
    }
}

// Add actor to scene from menu
void EditorManager::HeaderAddActor()
{
    if (ImGui::BeginMenu("Add Actor"))
    {
        std::vector<std::string> list = TemplateDB::ListAllTemplateTypes();

        std::sort(list.begin(), list.end());

        for (std::string name : list)
        {
            if (ImGui::MenuItem(name.c_str()))
            {
                SceneDB::Instantiate(name);
            }
        }
        ImGui::EndMenu();
    }
}

// Show component variables
void EditorManager::ShowComponent(luabridge::LuaRef& ref, const std::string& type)
{
    std::vector<std::string> keys = ComponentDB::GetKeys(type);
    int id = 0;
    for (const auto& key : keys)
    {
        luabridge::LuaRef value = ref[key];

        if (!value.isBool() && !value.isNumber() && !value.isString())
            continue;

        std::string label = key + ":";
        std::string name = "##" + key;
        ImGui::PushID(id++);

        ImGui::Text(label.c_str());
        ImGui::SameLine();


        if (value.isBool())
        {
            bool b0 = value.cast<bool>();

            if (ImGui::Checkbox(name.c_str(), &b0))
                ref[key] = b0;
        }
        else if (value.isNumber())
        {
            float f0 = value.cast<float>();

            ImGui::InputFloat(name.c_str(), &f0);

            if (ImGui::IsItemDeactivatedAfterEdit())
                ref[key] = f0;
        }
        else if (value.isString())
        {
            std::string val = value.cast<std::string>();
            char* str = &val[0];

            ImGui::InputText(name.c_str(), str, 50);

            if (ImGui::IsItemDeactivatedAfterEdit())
                ref[key] = std::string(str);
        }
        ImGui::PopID();
    }
}

void EditorManager::ShowPlayPauseStepButtons()
{
    bool is_open = true;
    ImGui::Begin("Controls", &is_open, ImGuiWindowFlags_NoTitleBar); // Ensures this window cant be hidden
    
    if (play_mode)
    {
        if (ImGui::Button("Stop"))
        {
            SceneDB::stopped = true;
            play_mode = false;
            SceneDB::RestartScene();
            SceneDB::LoadScene(start_scene);
        }
    }
    else
    {
        if (ImGui::Button("Play"))
        {
            ImGui::SetWindowFocus("");
            SaveScene();
            play_mode = true;
            SceneDB::stopped = paused;
            Helper::frame_number = 0;
            start_scene = SceneDB::GetCurrent();
            ComponentDB::ResetDatabase();
            ComponentManager::Init();
            SceneDB::RestartScene();
            SceneDB::LoadScene(start_scene);
        }
    }

    ImGui::SameLine();

    if (paused)
    {
        if (ImGui::Button("Resume"))
        {
            SceneDB::stopped = !play_mode;
            paused = false;
        }
    }
    else
    {
        if (ImGui::Button("Pause"))
        {
            SceneDB::stopped = true;
            paused = true;
        }
    }
   
    ImGui::End();
}

// Meny to save scene and load scene
void EditorManager::ShowSceneMenu()
{
    ImGui::Begin("Scene Files");
    
    if (ImGui::Button("Save Scene"))
        SaveScene();

    NewSceneButton();

    DisplayScenes();

    ImGui::End();
}

void EditorManager::NewSceneButton()
{
    bool pressed = false;

    pressed = ImGui::Button("New Scene");

    ImGui::SameLine();

    static std::string value = "";
    char* str = &value[0];
    if (ImGui::InputText("###NewScene", str, 50))
        value = std::string(str);
    
    if (pressed && !value.empty())
    {
        CreateScene(value);
        SceneDB::LoadScene(value);
        value = "";
    }
}

// Save scene to file
void EditorManager::SaveScene()
{
    // Start json document
    rapidjson::Document doc;
    doc.SetObject();
    std::string path = "resources/scenes/" + SceneDB::GetCurrent() + ".scene";
    
    AddSceneActorsToDocument(doc);
    WriteJsonFile(path, doc);
}

void EditorManager::AddSceneActorsToDocument(rapidjson::Document& doc)
{
    // Grab allocator from document
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    // Start actors array
    rapidjson::Value actors(rapidjson::kArrayType);

    // Loop through loaded actors to add to array
    for (const Actor* a : SceneDB::loaded_actors)
    {
        // Each actor is a json object
        rapidjson::Value a_json(rapidjson::kObjectType);

        ActorToJson(a, a_json, allocator);

        actors.PushBack(a_json, allocator);
    }
    doc.AddMember("actors", actors, allocator);
}

// Write a json document to file. Overwrites existing files
void EditorManager::WriteJsonFile(const std::string& path, rapidjson::Document& doc)
{
    // Create a file stream to write to a file
    FILE* file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "w");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif

    if (!file_pointer) {
        std::cerr << "error failed to open file [" << path << "]" << std::endl;
        exit(0);
    }

    const size_t buffer_size = 65536;
    char* buffer = new char[buffer_size];
    rapidjson::FileWriteStream stream(file_pointer, buffer, buffer_size);

    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
    doc.Accept(writer);

    std::fclose(file_pointer);
}

void EditorManager::ActorToJson(const Actor* a, rapidjson::Value& a_json, rapidjson::Document::AllocatorType& allocator)
{
    // Component list is a json component
    rapidjson::Value components(rapidjson::kObjectType);

    // Loop through components in order of key
    for (const auto& pair : a->actor_components)
    {
        const auto& c = pair.second;

        if (!c.alive)
            continue;

        // Each component is an object
        rapidjson::Value c_json(rapidjson::kObjectType);

        // Add type and enabled at the top
        c_json.AddMember("type", rapidjson::Value(c.type.c_str(), allocator), allocator);
        if (!c.IsEnabled())
            c_json.AddMember("enabled", false, allocator);

        if (c.type == "Rigidbody" || c.type == "ParticleSystem")
        {
            std::vector<std::string> keys = ComponentDB::GetKeys(c.type);
            for (const std::string& it_key : keys)
            {
                luabridge::LuaRef it_val = (*c.component_ref)[it_key];

                rapidjson::Value key_v(it_key.c_str(), allocator);
                if (it_val.isBool())
                    c_json.AddMember(key_v, it_val.cast<bool>(), allocator);
                else if (it_val.isNumber())
                    c_json.AddMember(key_v, it_val.cast<float>(), allocator);
                else if (it_val.isString())
                    c_json.AddMember(key_v, rapidjson::Value(it_val.tostring().c_str(), allocator), allocator);
            }
        }
        else
        {
            // Get all overrides from component
            for (luabridge::Iterator it(*c.component_ref); !it.isNil(); ++it)
            {
                std::string it_key = it.key().tostring();
                luabridge::LuaRef it_val = it.value();

                if (it_val.isFunction() || it_val.isUserdata())
                    continue;

                if (it_key == "key" || it_key == "type" || it_key == "enabled")
                    continue;

                rapidjson::Value key_v(it_key.c_str(), allocator);
                if (it_val.isBool())
                    c_json.AddMember(key_v, it_val.cast<bool>(), allocator);
                else if (it_val.isNumber())
                    c_json.AddMember(key_v, it_val.cast<float>(), allocator);
                else if (it_val.isString())
                    c_json.AddMember(key_v, rapidjson::Value(it_val.tostring().c_str(), allocator), allocator);
            }
        }
        rapidjson::Value key_c_json((*c.component_ref)["key"].tostring().c_str(), allocator);
        components.AddMember(key_c_json, c_json, allocator);
    }

    a_json.AddMember("name", rapidjson::Value(a->actor_name.c_str(), allocator), allocator);
    a_json.AddMember("components", components, allocator);
}

void EditorManager::CreateScene(const std::string& scene_name)
{
    scenes.push_back(scene_name);
    
    // Start json document
    rapidjson::Document doc;
    doc.SetObject();
    std::string path = "resources/scenes/" + scene_name + ".scene";

    // Grab allocator from document
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    // Start actors array
    rapidjson::Value actors(rapidjson::kArrayType);

    doc.AddMember("actors", actors, allocator);

    AddSceneActorsToDocument(doc);
    WriteJsonFile(path, doc);
}

void EditorManager::DisplayScenes()
{
    static int item_selected_idx = -1;

    ImGui::Text("Scene Selection");

    if (ImGui::BeginListBox("###CurrentScene"))
    {
        for (int n = 0; n < scenes.size(); n++)
        {
            const bool is_selected = (item_selected_idx == n);
            if (ImGui::Selectable(scenes[n].c_str(), is_selected))
            {
                if (item_selected_idx != n)
                {
                    item_selected_idx = n;
                    SceneDB::LoadScene(scenes[n]);
                }
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}
