#include "ComponentManager.h"

#include "Actor.h"
#include "AudioDB.h"
#include "EventBus.h"
#include "Input.h"
#include "ParticleSystem.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "SceneDB.h"
#include "TextDB.h"

#include "KeycodeToScancode.h"

#include <cstdlib>


// Initialize lua_state, functions, and components
void ComponentManager::Init()
{
    InitState();
    InitFunctions();
    ComponentDB::Init();
}

// Initalize lua_state
void ComponentManager::InitState()
{
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
}

// Inject the scripting API
void ComponentManager::InitFunctions()
{
    // Debug functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Debug")
        .addFunction("Log", &Print)
        .endNamespace();

    // Actor instances
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetID", &Actor::GetID)
        .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("GetComponents", &Actor::GetComponents)
        .addFunction("AddComponent", &Actor::AddComponent)
        .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass();

    // Actor functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Actor")
        .addFunction("Find", &SceneDB::Find)
        .addFunction("FindAll", &SceneDB::FindAll)
        .addFunction("Instantiate", &SceneDB::Instantiate)
        .addFunction("Destroy", &SceneDB::Destroy)
        .endNamespace();

    // Application functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Application")
        .addFunction("Quit", &Quit)
        .addFunction("Sleep", &Sleep)
        .addFunction("GetFrame", &GetFrame)
        .addFunction("OpenURL", &OpenURL)
        .endNamespace();

    // glm vec2 instances
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();

    // Input functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Input")
        .addFunction("GetKey", &GetKey)
        .addFunction("GetKeyDown", &GetKeyDown)
        .addFunction("GetKeyUp", &GetKeyUp)
        .addFunction("GetMousePosition", &Input::GetMousePosition)
        .addFunction("GetMouseButton", &Input::GetMouseButton)
        .addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
        .addFunction("HideCursor", &Input::HideCursor)
        .addFunction("ShowCursor", &Input::ShowCursor)
        .endNamespace();

    // Text functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Text")
        .addFunction("Draw", &Renderer::DrawText)
        .endNamespace();

    // Audio functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Audio")
        .addFunction("Play", &AudioDB::PlayChannel)
        .addFunction("Halt", &AudioDB::HaltChannel)
        .addFunction("SetVolume", &AudioDB::SetVolume)
        .endNamespace();

    // Image functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Image")
        .addFunction("DrawUI", &Renderer::DrawUI)
        .addFunction("DrawUIEx", &Renderer::DrawUIEx)
        .addFunction("Draw", &Renderer::Draw)
        .addFunction("DrawEx", &Renderer::DrawEx)
        .addFunction("DrawPixel", &Renderer::DrawPixel)
        .endNamespace();

    // Camera functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Camera")
        .addFunction("SetPosition", &Renderer::MoveCamera)
        .addFunction("GetPositionX", &Renderer::GetCameraX)
        .addFunction("GetPositionY", &Renderer::GetCameraY)
        .addFunction("SetZoom", &Renderer::SetZoom)
        .addFunction("GetZoom", &Renderer::GetZoom)
        .endNamespace();

    // Scene functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Scene")
        .addFunction("Load", &SceneDB::QueueLoad)
        .addFunction("GetCurrent", &SceneDB::GetCurrent)
        .addFunction("DontDestroy", &SceneDB::DontDestroy)
        .endNamespace();

    // Vector2 data type
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor <void (*) (void)>()
        .addConstructor <void (*) (float, float)>()
        .addProperty("x", &b2Vec2::x)
        .addProperty("y", &b2Vec2::y)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addFunction("__add", &b2Vec2::operator_add)
        .addFunction("__sub", &b2Vec2::operator_sub)
        .addFunction("__mul", &b2Vec2::operator_mul)
        .addStaticFunction("Distance", &b2Distance)
        .addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endClass();
    
    // Rigidbody class
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<Rigidbody>("Rigidbody")
        .addFunction("OnStart", &Rigidbody::OnStart)
        .addFunction("OnDestroy", &Rigidbody::OnDestroy)
        .addData("type", &Rigidbody::type)
        .addData("key", &Rigidbody::key)
        .addData("actor", &Rigidbody::actor)
        .addData("enabled", &Rigidbody::enabled)
        .addData("x", &Rigidbody::x)
        .addData("y", &Rigidbody::y)
        .addData("body_type", &Rigidbody::body_type)
        .addData("precise", &Rigidbody::precise)
        .addData("gravity_scale", &Rigidbody::gravity_scale)
        .addData("density", &Rigidbody::density)
        .addData("angular_friction", &Rigidbody::angular_friction)
        .addData("rotation", &Rigidbody::rotation)
        .addData("has_collider", &Rigidbody::has_collider)
        .addData("collider_type", &Rigidbody::collider_type)
        .addData("width", &Rigidbody::width)
        .addData("height", &Rigidbody::height)
        .addData("radius", &Rigidbody::radius)
        .addData("friction", &Rigidbody::friction)
        .addData("bounciness", &Rigidbody::bounciness)
        .addData("has_trigger", &Rigidbody::has_trigger)
        .addData("trigger_type", &Rigidbody::trigger_type)
        .addData("trigger_width", &Rigidbody::trigger_width)
        .addData("trigger_height", &Rigidbody::trigger_height)
        .addData("trigger_radius", &Rigidbody::trigger_radius)
        .addFunction("GetPosition", &Rigidbody::GetPosition)
        .addFunction("GetRotation", &Rigidbody::GetRotation)
        .addFunction("AddForce", &Rigidbody::AddForce)
        .addFunction("SetVelocity", &Rigidbody::SetVelocity)
        .addFunction("SetPosition", &Rigidbody::SetPosition)
        .addFunction("SetRotation", &Rigidbody::SetRotation)
        .addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
        .addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
        .addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
        .addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
        .addFunction("GetVelocity", &Rigidbody::GetVelocity)
        .addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
        .addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
        .addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
        .addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
        .endClass();

    // Collision class
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<Collision>("Collision")
        .addData("other", &Collision::other)
        .addData("point", &Collision::point)
        .addData("relative_velocity", &Collision::relative_velocity)
        .addData("normal", &Collision::normal)
        .endClass();

    // HitResult class
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<HitResult>("HitResult")
        .addData("actor", &HitResult::actor)
        .addData("point", &HitResult::point)
        .addData("normal", &HitResult::normal)
        .addData("is_trigger", &HitResult::is_trigger)
        .endClass();

    // Scene functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Physics")
        .addFunction("Raycast", &Rigidbody::Raycast)
        .addFunction("RaycastAll", &Rigidbody::RaycastAll)
        .endNamespace();

    // Scene functions
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Event")
        .addFunction("Publish", &EventBus::Publish)
        .addFunction("Subscribe", &EventBus::Subscribe)
        .addFunction("Unsubscribe", &EventBus::Unsubscribe)
        .endNamespace();

    // ParticleSystem class
    luabridge::getGlobalNamespace(lua_state)
        .beginClass<ParticleSystem>("ParticleSystem")
        .addFunction("OnStart", &ParticleSystem::OnStart)
        .addFunction("OnUpdate", &ParticleSystem::OnUpdate)
        .addData("type", &ParticleSystem::type)
        .addData("key", &ParticleSystem::key)
        .addData("actor", &ParticleSystem::actor)
        .addData("enabled", &ParticleSystem::enabled)
        .addData("x", &ParticleSystem::x)
        .addData("y", &ParticleSystem::y)
        .addData("frames_between_bursts", &ParticleSystem::frames_between_bursts)
        .addData("burst_quantity", &ParticleSystem::burst_quantity)
        .addData("start_scale_min", &ParticleSystem::start_scale_min)
        .addData("start_scale_max", &ParticleSystem::start_scale_max)
        .addData("rotation_min", &ParticleSystem::rotation_min)
        .addData("rotation_max", &ParticleSystem::rotation_max)
        .addData("start_color_r", &ParticleSystem::start_color_r)
        .addData("start_color_g", &ParticleSystem::start_color_g)
        .addData("start_color_b", &ParticleSystem::start_color_b)
        .addData("start_color_a", &ParticleSystem::start_color_a)
        .addData("emit_radius_min", &ParticleSystem::emit_radius_min)
        .addData("emit_radius_max", &ParticleSystem::emit_radius_max)
        .addData("emit_angle_min", &ParticleSystem::emit_angle_min)
        .addData("emit_angle_max", &ParticleSystem::emit_angle_max)
        .addData("image", &ParticleSystem::image)
        .addData("sorting_order", &ParticleSystem::sorting_order)
        .addData("duration_frames", &ParticleSystem::duration_frames)
        .addData("start_speed_min", &ParticleSystem::start_speed_min)
        .addData("start_speed_max", &ParticleSystem::start_speed_max)
        .addData("rotation_speed_min", &ParticleSystem::rotation_speed_min)
        .addData("rotation_speed_max", &ParticleSystem::rotation_speed_max)
        .addData("gravity_scale_x", &ParticleSystem::gravity_scale_x)
        .addData("gravity_scale_y", &ParticleSystem::gravity_scale_y)
        .addData("drag_factor", &ParticleSystem::drag_factor)
        .addData("angular_drag_factor", &ParticleSystem::angular_drag_factor)
        .addData("end_scale", &ParticleSystem::end_scale)
        .addData("end_color_r", &ParticleSystem::end_color_r)
        .addData("end_color_g", &ParticleSystem::end_color_g)
        .addData("end_color_b", &ParticleSystem::end_color_b)
        .addData("end_color_a", &ParticleSystem::end_color_a)
        .addFunction("Stop", &ParticleSystem::Stop)
        .addFunction("Play", &ParticleSystem::Play)
        .addFunction("Burst", &ParticleSystem::Burst)
        .endClass();
}

// Open URL
void ComponentManager::OpenURL(const std::string& url)
{
    std::string cmd;
#ifdef _WIN32
    cmd = "start " + url;
#elif __APPLE__
    cmd = "open " + url;
#else
    cmd = "xdg-open " + url;
#endif
    std::system(cmd.c_str());
}

// Get if key is pressed
bool ComponentManager::GetKey(const std::string& keycode)
{
    auto it = __keycode_to_scancode.find(keycode);
    if (it == __keycode_to_scancode.end())
        return false;
    return Input::GetKey(it->second);
}

// Get if key is pressed this frame
bool ComponentManager::GetKeyDown(const std::string& keycode)
{
    auto it = __keycode_to_scancode.find(keycode);
    if (it == __keycode_to_scancode.end())
        return false;
    return Input::GetKeyDown(it->second);
}

// Get if key is released this frame
bool ComponentManager::GetKeyUp(const std::string& keycode)
{
    auto it = __keycode_to_scancode.find(keycode);
    if (it == __keycode_to_scancode.end())
        return false;
    return Input::GetKeyUp(it->second);
}

// Component constructor
Component::Component()
{
    component_ref = std::make_shared<luabridge::LuaRef>(luabridge::newTable(lua_state));
    (*component_ref)["enabled"] = true;
    
    has_start = false;
    has_update = false;
    has_late_update = false;

    has_collision_enter = false;
    has_collision_exit = false;
    has_trigger_enter = false;
    has_trigger_exit = false;

    has_destroy = false;

    alive = true;
}

Component& Component::operator=(const Component& parent_component)
{
    luabridge::LuaRef& ref = *component_ref;
    type = parent_component.type;
    if (type == "Rigidbody")
    {
        ComponentDB::CreateCpp<Rigidbody>(ref, *parent_component.component_ref);
        has_start = true;
        has_destroy = true;
    }
    else if (type == "ParticleSystem")
    {
        ComponentDB::CreateCpp<ParticleSystem>(ref, *parent_component.component_ref);
        has_start = true;
        has_update = true;
    }
    else
    {
        // Establish inheritance to base component
        ComponentDB::EstablishInheritance(ref, *parent_component.component_ref);

        has_start = parent_component.has_start;
        has_update = parent_component.has_update;
        has_late_update = parent_component.has_late_update;

        has_collision_enter = parent_component.has_collision_enter;
        has_collision_exit = parent_component.has_collision_exit;
        has_trigger_enter = parent_component.has_trigger_enter;
        has_trigger_exit = parent_component.has_trigger_exit;

        has_destroy = parent_component.has_destroy;
    }
    return *this;
}

// Return true if component is enabled
bool Component::IsEnabled() const
{
    return (*component_ref)["enabled"];
}


