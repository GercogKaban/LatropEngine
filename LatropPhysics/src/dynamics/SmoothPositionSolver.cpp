#include "dynamics/SmoothPositionSolver.h"
#include "dynamics/RigidBody.h"

using namespace LP;

void SmoothPositionSolver::solve(const std::vector<Collision>& collisions, float deltaTime)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> deltas;

	for (const Collision& manifold : collisions) {
		RigidBody* aBody = dynamic_cast<RigidBody*>(manifold.bodyA);
        RigidBody* bBody = dynamic_cast<RigidBody*>(manifold.bodyB);

		float aInvMass = aBody->getInvMass();
        float bInvMass = bBody->getInvMass();

		const float percent = 0.8f;
		const float slop = 0.01f;

		glm::vec3 correction = manifold.points.normal * percent
			* fmax(manifold.points.depth - slop, 0.0f)
			/ (aInvMass + bInvMass);
	
		glm::vec3 deltaA;
		glm::vec3 deltaB;

		if (aBody ? aBody->isSimulated() : false) {
			deltaA = aInvMass * correction;
		}

		if (bBody ? bBody->isSimulated() : false) {
			deltaB = bInvMass * correction;
		}

		deltas.emplace_back(deltaA, deltaB);
	}

	for (unsigned i = 0; i < collisions.size(); i++) {
		RigidBody* aBody = collisions[i].bodyA->isSimulated() ? (RigidBody*)collisions[i].bodyA : nullptr;
		RigidBody* bBody = collisions[i].bodyB->isSimulated() ? (RigidBody*)collisions[i].bodyB : nullptr;

		if (aBody ? aBody->isSimulated() : false) {
			aBody->transform.position -= deltas[i].first;
		}

		if (bBody ? bBody->isSimulated() : false) {
			bBody->transform.position += deltas[i].second;
		}
	}
}