#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Actor.h"

#include "CollisionDetector.h"
#include "Raycast.h"

#include <string>


class Rigidbody
{
public:
    void OnStart();
    void OnDestroy();

    std::string type = "Rigidbody";
    std::string key = "???";
    Actor* actor = nullptr;
    bool enabled = true;

    static inline b2World* world = nullptr;
    static inline CollisionDetector detector;
    static void PhysicsStep();

    static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist);
    static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);

    static inline const float TO_RADIANS = b2_pi / 180.0f;
    static inline const float TO_DEGREES = 180.0f / b2_pi;

    b2Body* body = nullptr;
    
    float x = 0.0f;
    float y = 0.0f;
    std::string body_type = "dynamic";
    bool precise = true;
    float gravity_scale = 1.0f;
    float density = 1.0f;
    float angular_friction = 0.3f;
    float rotation = 0.0f;

    bool has_collider = true;
    std::string collider_type = "box";
    float width = 1.0f;
    float height = 1.0f;
    float radius = 0.5f;
    float friction = 0.3f;
    float bounciness = 0.3f;

    bool has_trigger = true;
    std::string trigger_type = "box";
    float trigger_width = 1.0f;
    float trigger_height = 1.0f;
    float trigger_radius = 0.5f;

    void SetPosition(b2Vec2 vec2);
    b2Vec2 GetPosition() const;

    void SetRotation(float degrees_clockwise);
    float GetRotation() const;

    void SetGravityScale(float gravity);
    float GetGravityScale() const;

    void AddForce(b2Vec2 vec2) { body->ApplyForceToCenter(vec2, true); }
    void SetVelocity(b2Vec2 vec2) { body->SetLinearVelocity(vec2); }
    void SetAngularVelocity(float degrees_clockwise) { body->SetAngularVelocity(degrees_clockwise * TO_RADIANS); }
    void SetUpDirection(b2Vec2 direction);
    void SetRightDirection(b2Vec2 direction);

    b2Vec2 GetVelocity() const { return body->GetLinearVelocity(); }
    float GetAngularVelocity() const { return body->GetAngularVelocity() * TO_DEGREES; }
    b2Vec2 GetUpDirection() const;
    b2Vec2 GetRightDirection() const;

private:
    void CreateBody();
    
    void CreateColliderFixture();
    void CreateTriggerFixture();
    void CreateDefaultFixture();
};

#endif
