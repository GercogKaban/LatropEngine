#pragma once

namespace LP 
{
    struct SphereCollider;
    struct BoundedPlaneCollider;
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

        // MARK: - Plane
        CollisionPoints findPlaneSphereCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const SphereCollider* b, const Transform* transformB
        );

        // MARK: - ABBB
        CollisionPoints findAABBAABBCollisionPoints(
            const AABBCollider* a, const Transform* transformA,
            const AABBCollider* b, const Transform* transformB
        );

        // MARK: Mixed
        CollisionPoints findSpherePlaneCollisionPoints(
            const SphereCollider* a, const Transform* transformA,
            const BoundedPlaneCollider* b, const Transform* transformB
        );

        CollisionPoints findPlaneAABBCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const AABBCollider* b, const Transform* transformB
        );
    }
} // namespace LP