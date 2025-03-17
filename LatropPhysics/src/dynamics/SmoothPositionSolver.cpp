#include "dynamics/SmoothPositionSolver.h"
#include "dynamics/RigidBody.h"

using namespace LatropPhysics;

void SmoothPositionSolver::solve(const std::vector<Collision>& collisions, float deltaTime)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> deltas;

		// for (Collision& manifold : collisions) {
		// 	RigidBody* aBody = manifold.ObjA->IsDynamic ? (RigidBody*)manifold.ObjA : nullptr;
		// 	RigidBody* bBody = manifold.ObjB->IsDynamic ? (RigidBody*)manifold.ObjB : nullptr;

		// 	float aInvMass = aBody ? aBody->InvMass : 0.0f;
		// 	float bInvMass = bBody ? bBody->InvMass : 0.0f;

		// 	const float percent = 0.8f;
		// 	const float slop = 0.01f;

		// 	glm::vec3 correction = manifold.Normal * percent
		// 		* fmax(manifold.Depth - slop, 0.0f)
		// 		/ (aInvMass + bInvMass);
		
		// 	glm::vec3 deltaA;
		// 	glm::vec3 deltaB;

		// 	if (aBody ? aBody->IsSimulated : false) {
		// 		deltaA = aInvMass * correction;
		// 	}

		// 	if (bBody ? bBody->IsSimulated : false) {
		// 		deltaB = bInvMass * correction;
		// 	}

		// 	deltas.emplace_back(deltaA, deltaB);
		// }

		// for (unsigned i = 0; i < manifolds.size(); i++) {
		// 	RigidBody* aBody = manifolds[i].ObjA->IsDynamic ? (RigidBody*)manifolds[i].ObjA : nullptr;
		// 	RigidBody* bBody = manifolds[i].ObjB->IsDynamic ? (RigidBody*)manifolds[i].ObjB : nullptr;

		// 	if (aBody ? aBody->IsSimulated : false) {
		// 		aBody->transform->position -= deltas[i].first;
		// 	}

		// 	if (bBody ? bBody->IsSimulated : false) {
		// 		bBody->transform->position += deltas[i].second;
		// 	}
		// }
}