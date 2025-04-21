#include "CollisionDetector.h"


void CollisionDetector::BeginContact(b2Contact* contact)
{
    b2Fixture* fixture_a = contact->GetFixtureA();
    b2Fixture* fixture_b = contact->GetFixtureB();
    Actor* actor_a = reinterpret_cast<Actor*>(fixture_a->GetUserData().pointer);
    Actor* actor_b = reinterpret_cast<Actor*>(fixture_b->GetUserData().pointer);
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    Collision collision;

    collision.other = actor_b;
    collision.point = world_manifold.points[0];
    collision.relative_velocity = fixture_a->GetBody()->GetLinearVelocity() - fixture_b->GetBody()->GetLinearVelocity();
    collision.normal = world_manifold.normal;

    if (fixture_a->IsSensor() && fixture_b->IsSensor())
    {
        collision.point = SENTINEL;
        collision.normal = SENTINEL;

        actor_a->TriggerEnter(collision);
        collision.other = actor_a;

        actor_b->TriggerEnter(collision);
    }
    else if (!fixture_a->IsSensor() && !fixture_b->IsSensor())
    {
        actor_a->CollisionEnter(collision);
        collision.other = actor_a;

        actor_b->CollisionEnter(collision);
    }
}


void CollisionDetector::EndContact(b2Contact* contact)
{
    b2Fixture* fixture_a = contact->GetFixtureA();
    b2Fixture* fixture_b = contact->GetFixtureB();
    Actor* actor_a = reinterpret_cast<Actor*>(fixture_a->GetUserData().pointer);
    Actor* actor_b = reinterpret_cast<Actor*>(fixture_b->GetUserData().pointer);
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    Collision collision;

    collision.other = actor_b;
    collision.point = SENTINEL;
    collision.relative_velocity = fixture_a->GetBody()->GetLinearVelocity() - fixture_b->GetBody()->GetLinearVelocity();
    collision.normal = SENTINEL;

    if (fixture_a->IsSensor() && fixture_b->IsSensor())
    {
        actor_a->TriggerExit(collision);
        collision.other = actor_a;

        actor_b->TriggerExit(collision);
    }
    else if (!fixture_a->IsSensor() && !fixture_b->IsSensor())
    {
        actor_a->CollisionExit(collision);
        collision.other = actor_a;

        actor_b->CollisionExit(collision);
    }
}
