#include "scenes/Shared.h"
#include "LPlayerCharacter.h"
#include "LEngine.h"
#include <collision/BoundedPlaneCollider.h>

// MARK: - Shared Colliders
std::shared_ptr<LP::OBBCollider> cubeOBBCollider = std::make_shared<LP::OBBCollider>(LP::OBBCollider::makeCube());
std::shared_ptr<LP::BoundedPlaneCollider> planeYUPCollider = std::make_shared<LP::BoundedPlaneCollider>(LP::BoundedPlaneCollider());

void SharedScene::createPlayer(glm::vec3 origin, bool takesGravity) 
{
	auto weakPlayer = ObjectBuilder::construct<LPlayerCharacter>();
	auto playerCharacter = weakPlayer.lock();
	playerCharacter->loadComponent<LP::RigidBody>([weakPlayer, origin, takesGravity](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(true);
			physicsComponent->transform.scale = LPlayerCharacter::standingDimensions;
			physicsComponent->setMass(LPlayerCharacter::mass);
			physicsComponent->takesGravity = takesGravity;
			physicsComponent->freezesRotation = true;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = origin;
			physicsComponent->material = LP::Material::HumanBody;
			physicsComponent->material.frictionCombinator = LP::Material::CombinationMode::Minimum;
			physicsComponent->onCollision = [weakPlayer](LP::CollisionManifold collision, float depth) {
				if (collision.normal.y > 0)
				{
					auto playerCharacter = weakPlayer.lock();
					playerCharacter->resetJump();
				}
			};
		});
}

void SharedScene::createCube(glm::vec3 origin, bool takesGravity, std::string &&texturePath)
{
	auto cubeD = ObjectBuilder::construct<LActor>().lock();
	cubeD->loadComponent<LG::LCube>();
    cubeD->graphicsComponent->setColorTexture(std::move(texturePath));
	cubeD->loadComponent< LP::RigidBody>([origin, takesGravity](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(takesGravity);
			if (takesGravity) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = takesGravity;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->transform.position = origin;
		});
}

void decorate(glm::vec3 origin, glm::quat rotation, glm::vec3 scale)
{
	auto finalRotation = rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
	finalRotation = finalRotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(1, 0, 0));

	auto floor = ObjectBuilder::construct<LActor>().lock();
		floor->loadComponent<LG::LPlane>();
		floor->loadComponent<LP::RigidBody>([origin, finalRotation, scale](LP::RigidBody *physicsComponent) 
		{
			physicsComponent->setIsSimulated(false);

			physicsComponent->transform.rotation *= finalRotation;
			physicsComponent->transform.scale = scale;
			physicsComponent->transform.position = origin;
		});
		floor->graphicsComponent->setColorTexture("textures/PavingStones138.jpg");
}

void SharedScene::createBluePortal(glm::vec3 origin, glm::quat rotation, glm::vec3 scale, bool isDecorated)
{
	glm::quat rotationY = rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));

	auto bluePortal = ObjectBuilder::construct<LActor>().lock();
	bluePortal->loadComponent<LG::BluePortal>([origin](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::BluePortal*>(graphicsComponent)->setPortalView(glm::lookAt(origin, origin + cameraFront, cameraUp));
		});

	bluePortal->loadComponent<LP::RigidBody>([origin, scale, rotationY](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			auto player = LPlayerCharacter::get();
			player->bluePortal = physicsComponent;

			physicsComponent->collider = planeYUPCollider;
			physicsComponent->transform.position = origin;
			physicsComponent->transform.scale = scale;
			physicsComponent->transform.rotation *= rotationY;
			physicsComponent->isTrigger = true;

			glm::vec3 portalNormal = rotationY * LP::BoundedPlaneCollider::normal;

			physicsComponent->onCollision = [player, portalNormal](LP::CollisionManifold collision, float dt) {
				glm::vec3 bodyNormal = collision.bodyB->transform.position - collision.bodyA->transform.position;
				if (glm::dot(portalNormal, bodyNormal) < 0)
				{
					player->teleportThroughPortal(player->bluePortal, player->orangePortal);
				}
			};
		});

	if (isDecorated) decorate(origin, rotation, scale);
}

void SharedScene::createOrangePortal(glm::vec3 origin, glm::quat rotation, glm::vec3 scale, bool isDecorated)
{
	glm::quat rotationY = rotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));

	auto orangePortal = ObjectBuilder::construct<LActor>().lock();
	orangePortal->loadComponent<LG::OrangePortal>([origin](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::OrangePortal*>(graphicsComponent)->setPortalView(glm::lookAt(origin, origin + cameraFront, cameraUp));
		});

	orangePortal->loadComponent<LP::RigidBody>([origin, scale, rotationY](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			auto player = LPlayerCharacter::get();
			player->orangePortal = physicsComponent;

			physicsComponent->collider = planeYUPCollider;
			physicsComponent->transform.position = origin;
			physicsComponent->transform.scale = scale;
			physicsComponent->transform.rotation *= rotationY;
			physicsComponent->isTrigger = true;

			glm::vec3 portalNormal = rotationY * LP::BoundedPlaneCollider::normal;

			physicsComponent->onCollision = [player, portalNormal](LP::CollisionManifold collision, float dt) {
				glm::vec3 bodyNormal = collision.bodyB->transform.position - collision.bodyA->transform.position;
				if (glm::dot(portalNormal, bodyNormal) < 0)
				{
					player->teleportThroughPortal(player->orangePortal, player->bluePortal);
				}
			};
		});

	if (isDecorated) decorate(origin, rotation, scale);
}