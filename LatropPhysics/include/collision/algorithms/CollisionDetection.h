#pragma once

namespace LP 
{
    struct SphereCollider;
    struct CapsuleCollider;
    struct BoundedPlaneCollider;
    struct AABBCollider;
    struct OBBCollider;
    struct Transform;
    struct CollisionPoints;

    namespace collisionDetectors 
    {
        // MARK: - Sphere

        CollisionPoints findSphereSphereCollisionPoints(
            const SphereCollider* a, const Transform* transformA,
            const SphereCollider* b, const Transform* transformB
        );
        
        // MARK: - Capsule

        CollisionPoints findCapsuleCapsuleCollisionPoints(
            const CapsuleCollider* a, const Transform* transformA,
            const CapsuleCollider* b, const Transform* transformB
        );

        // MARK: - ABBB

        CollisionPoints findAABBAABBCollisionPoints(
            const AABBCollider* a, const Transform* transformA,
            const AABBCollider* b, const Transform* transformB
        );

        // MARK: - OBB

        CollisionPoints findOBBOBBCollisionPoints(
            const OBBCollider* a, const Transform* transformA,
            const OBBCollider* b, const Transform* transformB
        );

        // MARK: Mixed - Plane

        CollisionPoints findPlaneSphereCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const SphereCollider* b, const Transform* transformB
        );

        CollisionPoints findPlaneCapsuleCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const CapsuleCollider* b, const Transform* transformB
        );

        CollisionPoints findPlaneAABBCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const AABBCollider* b, const Transform* transformB
        );

        // MARK: Mixed - AABB

        CollisionPoints findAABBCapsuleCollisionPoints(
            const AABBCollider* a, const Transform* transformA,
            const CapsuleCollider* b, const Transform* transformB
        );
    }
} // namespace LP