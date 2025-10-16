#pragma once

namespace LP 
{
    struct SphereCollider;
    struct CapsuleCollider;
    struct BoundedPlaneCollider;
    struct OBBCollider;
    struct Transform;
    struct ContactManifold;
    struct AABB;

    namespace collisionDetectors 
    {
        // MARK: - ABBB

        bool testAABBAABBForCollision(const AABB& a, const AABB& b);

        // MARK: - Sphere

        ContactManifold findSphereSphereCollisionPoints(
            const SphereCollider* a, const Transform* transformA,
            const SphereCollider* b, const Transform* transformB
        );
        
        // MARK: - Capsule

        ContactManifold findCapsuleCapsuleCollisionPoints(
            const CapsuleCollider* a, const Transform* transformA,
            const CapsuleCollider* b, const Transform* transformB
        );

        // MARK: - OBB

        ContactManifold findOBBOBBCollisionPoints(
            const OBBCollider* a, const Transform* transformA,
            const OBBCollider* b, const Transform* transformB
        );

        // MARK: Mixed - Plane

        ContactManifold findPlaneSphereCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const SphereCollider* b, const Transform* transformB
        );

        ContactManifold findPlaneCapsuleCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const CapsuleCollider* b, const Transform* transformB
        );

        ContactManifold findPlaneOBBCollisionPoints(
            const BoundedPlaneCollider* a, const Transform* transformA,
            const OBBCollider* b, const Transform* transformB
        );
    }
} // namespace LP