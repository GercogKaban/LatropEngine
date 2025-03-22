#include "dynamics/SmoothPositionSolver.h"
#include "dynamics/RigidBody.h"

using namespace LP;

void SmoothPositionSolver::solve(const std::vector<Collision>& collisions, float deltaTime)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> deltas;

	for (const Collision& manifold : collisions) {
		RigidBody* aBody = manifold.bodyA->m_isSimulated ? (RigidBody*)manifold.bodyA : nullptr;
		RigidBody* bBody = manifold.bodyB->m_isSimulated ? (RigidBody*)manifold.bodyB : nullptr;

		float aInvMass = aBody ? 1 / aBody->m_mass : 1.0f;
        float bInvMass = bBody ? 1 / bBody->m_mass : 1.0f;

		const float percent = 0.8f;
		const float slop = 0.01f;

		glm::vec3 correction = manifold.points.normal * percent
			* fmax(manifold.points.depth - slop, 0.0f)
			/ (aInvMass + bInvMass);
	
		glm::vec3 deltaA;
		glm::vec3 deltaB;

		if (aBody ? aBody->m_isSimulated : false) {
			deltaA = aInvMass * correction;
		}

		if (bBody ? bBody->m_isSimulated : false) {
			deltaB = bInvMass * correction;
		}

		deltas.emplace_back(deltaA, deltaB);
	}

	for (unsigned i = 0; i < collisions.size(); i++) {
		RigidBody* aBody = collisions[i].bodyA->m_isSimulated ? (RigidBody*)collisions[i].bodyA : nullptr;
		RigidBody* bBody = collisions[i].bodyB->m_isSimulated ? (RigidBody*)collisions[i].bodyB : nullptr;

		if (aBody ? aBody->m_isSimulated : false) {
			aBody->transform.position -= deltas[i].first;
		}

		if (bBody ? bBody->m_isSimulated : false) {
			bBody->transform.position += deltas[i].second;
		}
	}
}