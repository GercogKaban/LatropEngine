#pragma once

namespace LatropPhysics 
{
    struct SphereCollider;
    struct PlaneCollider;
    struct AABBCollider;
    struct Transform;
    struct CollisionPoints;

    namespace collisionDetectors 
    {
        // MARK: - Sphere
        CollisionPoints findSphereSphereCollisionPoints(
            const SphereCollider* a, const Transform* transformA,
            const SphereCollider* b, const Transform* transformB
        );

        CollisionPoints findSpherePlaneCollisionPoints(
            const SphereCollider* a, const Transform* transformA,
            const PlaneCollider* b, const Transform* transformB
        );

        // MARK: - Plane
        CollisionPoints findPlaneSphereCollisionPoints(
            const PlaneCollider* a, const Transform* transformA,
            const SphereCollider* b, const Transform* transformB
        );

        // MARK: - ABBB
        CollisionPoints findAABBAABBCollisionPoints(
            const AABBCollider* a, const Transform* transformA,
            const AABBCollider* b, const Transform* transformB
        );

        CollisionPoints findAABBPlaneCollisionPoints(
            const AABBCollider* a, const Transform* transformA,
            const AABBCollider* b, const Transform* transformB
        );
    }
} // namespace LatropPhysics