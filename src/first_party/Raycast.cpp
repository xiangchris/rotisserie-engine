#include "Raycast.h"


float RaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& contact_point, const b2Vec2& contact_normal, float fraction)
{
    Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);

    if (!actor)
        return -1.0f;
    
    hitresult.actor = actor;
    hitresult.point = contact_point;
    hitresult.normal = contact_normal;
    hitresult.is_trigger = fixture->IsSensor();
    
    return fraction;
}

float RaycastCallbackAll::ReportFixture(b2Fixture* fixture, const b2Vec2& contact_point, const b2Vec2& contact_normal, float fraction)
{
    Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);

    if (!actor)
        return -1.0f;

    HitResult& hitresult = hitresult_list.emplace_back();
    hitresult.actor = actor;
    hitresult.point = contact_point;
    hitresult.normal = contact_normal;
    hitresult.is_trigger = fixture->IsSensor();
    hitresult.fraction = fraction;

    return 1.0f;
}
