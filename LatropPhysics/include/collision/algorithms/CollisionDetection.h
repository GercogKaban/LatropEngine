#pragma once

namespace LP 
{
    struct SphereCollider;
    struct CapsuleCollider;
    struct BoundedPlaneCollider;
    struct OBBCollider;
    struct Transform;
    struct CollisionPoints;
    struct AABB;

    namespace collisionDetectors 
    {
        // MARK: - ABBB

        bool testAABBAABBForCollision(const AABB& a, const AABB& b);

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
    }
} // namespace LP