#include "scenes/Shared.h"
#include "LPlayerCharacter.h"
#include "LEngine.h"

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
			physicsComponent->setMass(LPlayerCharacter::mass);
			physicsComponent->takesGravity = takesGravity;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = origin;
			// physicsComponent->transform.scale = LPlayerCharacter::standingDimensions;
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

void SharedScene::createPortals()
{
	const glm::vec3 portalScale = glm::vec3(1.6f, 0.01f, 2.8f);
	glm::quat rotationY = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));

	const glm::vec3 pos1 = glm::vec3(7.0f, 1.0f, -5.0f);
	const glm::vec3 pos2 = glm::vec3(-5.0f, 1.0f, -5.0f);

	auto bluePortal = ObjectBuilder::construct<LActor>().lock();
	bluePortal->loadComponent<LG::LPortal>([pos1](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::LPortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos1, pos1 + cameraFront, cameraUp));

			auto p1View = glm::lookAt(pos1, pos1 + cameraFront, cameraUp);

			auto playerView = LRenderer::get()->getView();

			//glm::mat4 model(1.0f);
			//model = glm::translate(model, {0,0,5});

			auto modifiedModel = p1View * playerView;

			return;
		});
	bluePortal->loadComponent<LP::RigidBody>([pos1, portalScale, rotationY](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			auto player = LPlayerCharacter::get();
			player->bluePortal = physicsComponent;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = pos1;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::CollisionManifold collision, float dt) {
				if (collision.normal.z == 1)
				{
					// std::cout << "Colliding: ";
					// std::cout << "x: " << collision.points.normal.x << " ";
					// std::cout << "y: " << collision.points.normal.y << " ";
					// std::cout << "z: " << collision.points.normal.z << " ";
					// std::cout << "d: " << collision.points.depth << " ";
					// std::cout << std::endl;
					// auto portal = player->orangePortal;
					// // Get destination portal's transform
					// auto destinationTransform = portal->transform;
					// // destinationTransform.position.y -= 1.0f;

					// // Compute teleport position: Move to portal position & offset slightly along normal
					// glm::vec3 portalNormal = glm::normalize(destinationTransform.rotation * glm::vec3 { 0.0f, 1.0f, 0.0f });
					// // Teleport position
					// player->physicsComponent->transform.position = destinationTransform.position + portalNormal/* * 1.1f */; // Offset slightly to prevent instant re-trigger
					// player->physicsComponent->linearVelocity = glm::reflect(player->physicsComponent->linearVelocity, portalNormal);

					// // Rotate player 180 degrees around Y-axis
					// player->setOrientation(player->orientation * glm::angleAxis(glm::radians(180.0f) ,glm::vec3(0, 1, 0)));
				}
			};
		});

	auto orangePortal = ObjectBuilder::construct<LActor>().lock();
	orangePortal->loadComponent<LG::LPortal>([pos2](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::LPortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos2, pos2 + cameraFront, cameraUp));
		});
	orangePortal->loadComponent<LP::RigidBody>([pos2, portalScale, rotationY](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			auto player = LPlayerCharacter::get();
			player->orangePortal = physicsComponent;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = pos2;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::CollisionManifold collision, float dt) {
				if (collision.normal.z == -1)
				{
					// std::cout << "Colliding: ";
					// std::cout << "x: " << collision.points.normal.x << " ";
					// std::cout << "y: " << collision.points.normal.y << " ";
					// std::cout << "z: " << collision.points.normal.z << " ";
					// std::cout << "d: " << collision.points.depth << " ";
					// std::cout << std::endl;
					// auto portal = player->bluePortal;
					// // Get destination portal's transform
					// auto destinationTransform = portal->transform;
					// // destinationTransform.position.y -= 1.0f;

					// // Compute teleport position: Move to portal position & offset slightly along normal
					// glm::vec3 portalNormal = glm::normalize(destinationTransform.rotation * glm::vec3{ 0.0f, 1.0f, 0.0f });
					// // Teleport position
					// player->physicsComponent->transform.position = destinationTransform.position + portalNormal/* * 1.1f */; // Offset slightly to prevent instant re-trigger
					// player->physicsComponent->linearVelocity = glm::reflect(player->physicsComponent->linearVelocity, portalNormal);

					// // Rotate player 180 degrees around Y-axis
					// player->setOrientation(player->orientation * glm::angleAxis(glm::radians(180.0f) ,glm::vec3(0, 1, 0)));
				}
			};
		});
}