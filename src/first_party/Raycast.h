#ifndef RAYCAST_H
#define RAYCAST_H

#include "Actor.h"

#include "box2d/box2d.h"

class Actor;

class HitResult
{
public:
    Actor* actor = nullptr;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger = false;
    float fraction = 1.0f;
};
struct CompareHitResults {
    bool operator()(const HitResult& lhs, const HitResult& rhs) const {
        return lhs.fraction < rhs.fraction;
    }
};

class RaycastCallback : public b2RayCastCallback
{
public:
    float ReportFixture(b2Fixture* fixture, const b2Vec2& contact_point, const b2Vec2& contact_normal, float fraction) override;

    HitResult hitresult;
    float nearest_fraction = 1.0f;
};

class RaycastCallbackAll : public b2RayCastCallback
{
public:
    float ReportFixture(b2Fixture* fixture, const b2Vec2& contact_point, const b2Vec2& contact_normal, float fraction) override;

    std::deque<HitResult> hitresult_list;
};

#endif