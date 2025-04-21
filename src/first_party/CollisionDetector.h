#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include "Actor.h"

#include "box2d/box2d.h"

class Actor;

class CollisionDetector : public b2ContactListener
{
    static inline const b2Vec2 SENTINEL = b2Vec2(-999.0f, -999.0f);
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
};

class Collision
{
public:
    Actor* other = nullptr;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

#endif
