#pragma once
struct SphereCollider;
struct PlaneCollider;
struct Transform;
struct CollisionPoints;

namespace collisionDetectors {
    CollisionPoints findSphereSphereCollisionPoints(
        const SphereCollider* a, const Transform* transformA,
        const SphereCollider* b, const Transform* transformB
    );

    CollisionPoints findSpherePlaneCollisionPoints(
        const SphereCollider* a, const Transform* transformA,
        const PlaneCollider* b, const Transform* transformB
    );

    CollisionPoints findPlaneSphereCollisionPoints(
        const PlaneCollider* a, const Transform* transformA,
        const SphereCollider* b, const Transform* transformB
    );
}