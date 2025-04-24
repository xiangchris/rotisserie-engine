#include "Rigidbody.h"

#include "LuaAPI.h"
#include "Raycast.h"

#include "glm/glm.hpp"


// Update physics world
void Rigidbody::PhysicsStep()
{
    if (world)
        world->Step(1.0f / 60.0f, 8, 3);
}

// Initialize Rigidbody after reading in values
void Rigidbody::OnStart()
{
    if (!world)
    {
        world = new b2World(b2Vec2(0.0f, 9.8f));
        world->SetContactListener(&detector);
    }

    CreateBody();

    if (has_collider)
        CreateColliderFixture();
    if (has_trigger)
        CreateTriggerFixture();
    if (!has_collider && !has_trigger)
        CreateDefaultFixture();
}

// Create body and add to world
void Rigidbody::CreateBody()
{
    b2BodyDef body_def;
    if (body_type == "dynamic")
        body_def.type = b2_dynamicBody;
    else if (body_type == "kinematic")
        body_def.type = b2_kinematicBody;
    else
        body_def.type = b2_staticBody;

    body_def.position.Set(x, y);
    body_def.angle = rotation * TO_RADIANS;
    body_def.bullet = precise;
    body_def.angularDamping = angular_friction;
    body_def.gravityScale = gravity_scale;

    body = world->CreateBody(&body_def);
}

// Create collider fixture type and add to body
void Rigidbody::CreateColliderFixture()
{
    b2Shape* shape = nullptr;

    if (collider_type == "box")
    {
        b2PolygonShape* polygon_shape = new b2PolygonShape();
        polygon_shape->SetAsBox(width * 0.5f, height * 0.5f);
        shape = polygon_shape;
    }
    else if (collider_type == "circle")
    {
        b2CircleShape* circle_shape = new b2CircleShape();
        circle_shape->m_radius = radius;
        shape = circle_shape;
    }

    b2FixtureDef fixture_def;
    fixture_def.shape = shape;
    fixture_def.density = density;
    fixture_def.restitution = bounciness;
    fixture_def.friction = friction;
    fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);

    fixture_def.filter.groupIndex = 1;
    fixture_def.filter.categoryBits = 0x0001;
    fixture_def.filter.maskBits = 0x0001;

    fixture_def.isSensor = false;
    body->CreateFixture(&fixture_def);
}

// Create trigger fixture type and add to body
void Rigidbody::CreateTriggerFixture()
{
    b2Shape* shape = nullptr;

    if (trigger_type == "box")
    {
        b2PolygonShape* polygon_shape = new b2PolygonShape();
        polygon_shape->SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
        shape = polygon_shape;
    }
    else if (trigger_type == "circle")
    {
        b2CircleShape* circle_shape = new b2CircleShape();
        circle_shape->m_radius = trigger_radius;
        shape = circle_shape;
    }

    b2FixtureDef fixture_def;
    fixture_def.shape = shape;
    fixture_def.density = density;
    fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);

    fixture_def.filter.groupIndex = 2;
    fixture_def.filter.categoryBits = 0x0002;
    fixture_def.filter.maskBits = 0x0002;

    fixture_def.isSensor = true;
    body->CreateFixture(&fixture_def);
}

// Create fixture for neither trigger nor collider and add to body
void Rigidbody::CreateDefaultFixture()
{
    b2PolygonShape phantom_shape;
    phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);

    b2FixtureDef fixture_def;
    fixture_def.shape = &phantom_shape;
    fixture_def.density = density;

    fixture_def.filter.groupIndex = -1;

    fixture_def.isSensor = true;
    body->CreateFixture(&fixture_def);
}

// On destruction, remove body from world
void Rigidbody::OnDestroy()
{
    world->DestroyBody(body);
}

// If body has not been initialized, overwrite initial values
void Rigidbody::SetPosition(b2Vec2 vec2)
{
    if (!body)
    {
        x = vec2.x;
        y = vec2.y;
    }
    else
        body->SetTransform(vec2, body->GetAngle());
}

// If body has not been initialized, return initial values
b2Vec2 Rigidbody::GetPosition() const
{
    if (!body)
        return b2Vec2(x, y);
    return body->GetPosition();
}

// If body has not been initialized, overwrite initial values
void Rigidbody::SetRotation(float degrees_clockwise)
{
    if (!body)
        rotation = degrees_clockwise;
    else
        body->SetTransform(body->GetPosition(), degrees_clockwise * TO_RADIANS);
}

// If body has not been initialized, return initial values
float Rigidbody::GetRotation() const
{
    if (!body)
        return rotation;
    else
        return body->GetAngle() * TO_DEGREES;
}

// If body has not been initialized, overwrite initial values
void Rigidbody::SetGravityScale(float gravity)
{
    if (!body)
        gravity_scale = gravity;
    else
        body->SetGravityScale(gravity);
}

// If body has not been initialized, return initial values
float Rigidbody::GetGravityScale() const
{
    if (!body)
        return gravity_scale;
    else
        return body->GetGravityScale();
}

// Performs a rotation of the rigidbody such that its local "up" vector points in a certain direction.
void Rigidbody::SetUpDirection(b2Vec2 direction)
{
    direction.Normalize();
    body->SetTransform(body->GetPosition(), glm::atan(direction.x, -direction.y));
}

// Performs a rotation of the rigidbody such that its local "right" vector points in a certain direction.
void Rigidbody::SetRightDirection(b2Vec2 direction)
{
    direction.Normalize();
    body->SetTransform(body->GetPosition(), glm::atan(direction.x, -direction.y) - (b2_pi * 0.5f));
}

// This is the vector that points locally "up" out of a body, changing as it rotates
b2Vec2 Rigidbody::GetUpDirection() const
{
    float angle = body->GetAngle();
    b2Vec2 result = b2Vec2(glm::sin(angle), -glm::cos(angle));
    result.Normalize();
    return result;
}

// This is the vector that points locally "right" out of a body, changing as it rotates
b2Vec2 Rigidbody::GetRightDirection() const
{
    float angle = body->GetAngle();
    b2Vec2 result = b2Vec2(glm::cos(angle), glm::sin(angle));
    result.Normalize();
    return result;
}

luabridge::LuaRef Rigidbody::Raycast(b2Vec2 pos, b2Vec2 dir, float dist)
{
    if (!world || dist == 0)
        return luabridge::LuaRef(LuaAPI::GetLuaState());
    
    dir.Normalize();
    dir *= dist;
    dir += pos;
    
    RaycastCallback callback;
    world->RayCast(&callback, pos, dir);

    if (!callback.hitresult.actor)
        return luabridge::LuaRef(LuaAPI::GetLuaState());

    return luabridge::LuaRef(LuaAPI::GetLuaState(), callback.hitresult);
}

luabridge::LuaRef Rigidbody::RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist)
{
    if (!world || dist == 0)
        return luabridge::LuaRef(LuaAPI::GetLuaState());

    dir.Normalize();
    dir *= dist;
    dir += pos;

    RaycastCallbackAll callback;
    world->RayCast(&callback, pos, dir);

    luabridge::LuaRef table = luabridge::newTable(LuaAPI::GetLuaState());
    if (callback.hitresult_list.empty())
        return table;

    int i = 1;
    std::sort(callback.hitresult_list.begin(), callback.hitresult_list.end(), CompareHitResults());
    for (const HitResult& hit : callback.hitresult_list)
    {
        table[i++] = hit;
    }

    return table;
}
