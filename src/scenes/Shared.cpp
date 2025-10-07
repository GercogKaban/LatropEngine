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

void SharedScene::createPortals()
{
	const glm::vec3 portalScale = glm::vec3(1.6f, 0.01f, 2.8f);
	glm::quat rotationY = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));

	const glm::vec3 pos1 = glm::vec3(7.0f, 10.0f, -5.0f);
	const glm::vec3 pos2 = glm::vec3(-5.0f, 1.0f, -5.0f);

	auto bluePortal = ObjectBuilder::construct<LActor>().lock();
	bluePortal->loadComponent<LG::BluePortal>([pos1](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::BluePortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos1, pos1 + cameraFront, cameraUp));

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
				if (collision.normal.z == 1/* && collision.depth > 0.000001*/)
				{
					// std::cout << "Colliding: ";
					// std::cout << "x: " << collision.normal.x << " ";
					// std::cout << "y: " << collision.normal.y << " ";
					// std::cout << "z: " << collision.normal.z << " ";
					// std::cout << "d: " << collision.depth << " ";
					// std::cout << std::endl;

					// player->teleportTo(player->orangePortal);
					// player->teleportThroughPortal(player->bluePortal, player->orangePortal);
				}
			};
		});

	auto orangePortal = ObjectBuilder::construct<LActor>().lock();
	orangePortal->loadComponent<LG::OrangePortal>([pos2](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::OrangePortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos2, pos2 + cameraFront, cameraUp));
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
				if (collision.normal.z == 1/* && collision.depth > 0.001*/)
				{
					// std::cout << "Colliding: ";
					// std::cout << "x: " << collision.normal.x << " ";
					// std::cout << "y: " << collision.normal.y << " ";
					// std::cout << "z: " << collision.normal.z << " ";
					// std::cout << "d: " << collision.depth << " ";
					// std::cout << std::endl;
					// player->teleportTo(player->bluePortal);
					// player->teleportThroughPortal(player->orangePortal, player->bluePortal);
				}
			};
		});
}

void SharedScene::createCube(glm::vec3 origin, bool takesGravity, std::__1::string &&texturePath)
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