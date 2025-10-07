#include "scenes/Shared.h"
#include "scenes/RoomScene.h"
#include "scenes/PortalsSandbox.h"
#include "LEngine.h"
#include "LPlayerCharacter.h"

void PortalsSandbox::createSymmetricScenario()
{
    SharedScene::createPlayer({ -4, 2, -3.5 }, true);
    RoomScene::createFloor(400, 400);

    // MARK: Cubes
    bool isSimulated = false;
    auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
    cubeA->graphicsComponent->setColorTexture("textures/smile2.jpg");
	cubeA->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 1.0f;
			physicsComponent->transform.position = glm::vec3(6.0f, 1.0f, -2.0f);
		});

    auto cubeB = ObjectBuilder::construct<LActor>().lock();
	cubeB->loadComponent<LG::LCube>();
    cubeB->graphicsComponent->setColorTexture("textures/smile2.jpg");
	cubeB->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 1.0f;
			physicsComponent->transform.position = glm::vec3(3.0f, 1.0f, -1.0f);
		});

    auto cubeC = ObjectBuilder::construct<LActor>().lock();
	cubeC->loadComponent<LG::LCube>();
    cubeC->graphicsComponent->setColorTexture("textures/smile2.jpg");
	cubeC->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 1.0f;
			physicsComponent->transform.position = glm::vec3(-3.0f, 1.0f, -9.0f);
		});

    auto cubeD = ObjectBuilder::construct<LActor>().lock();
	cubeD->loadComponent<LG::LCube>();
    cubeD->graphicsComponent->setColorTexture("textures/smile2.jpg");
	cubeD->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 1.0f;
			physicsComponent->transform.position = glm::vec3(-6.0f, 1.0f, -8.0f);
		});

    // MARK: Portals
    const glm::vec3 portalScale = glm::vec3(1.6f, 0.01f, 2.8f);
	glm::quat rotationY1 = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));

	const glm::vec3 pos1 = glm::vec3(4.0f, 1.5f, -5.0f);
	const glm::vec3 pos2 = glm::vec3(-4.0f, 1.5f, -5.0f);

	auto bluePortal = ObjectBuilder::construct<LActor>().lock();
	bluePortal->loadComponent<LG::LPortal>([pos1](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::LPortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos1, pos1 + cameraFront, cameraUp));

			auto p1View = glm::lookAt(pos1, pos1 + cameraFront, cameraUp);

			auto playerView = LRenderer::get()->getView();

			auto modifiedModel = p1View * playerView;

			return;
		});
	bluePortal->loadComponent<LP::RigidBody>([pos1, portalScale, rotationY1](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			auto player = LPlayerCharacter::get();
			player->bluePortal = physicsComponent;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = pos1;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY1;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::CollisionManifold collision, float dt) {};
		});

	auto orangePortal = ObjectBuilder::construct<LActor>().lock();
	orangePortal->loadComponent<LG::LPortal>([pos2](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::LPortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos2, pos2 + cameraFront, cameraUp));
		});
	orangePortal->loadComponent<LP::RigidBody>([pos2, portalScale, rotationY1](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			auto player = LPlayerCharacter::get();
			player->orangePortal = physicsComponent;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = pos2;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY1;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::CollisionManifold collision, float dt) {};
		});
}

void PortalsSandbox::createRotatedScenario()
{
	SharedScene::createPlayer();
    RoomScene::createFloor(400, 400);

	// MARK: Stairs
	RoomScene::createSmoothStairs(600, { 6, -1, 2 }, 4.5);
	RoomScene::createPerfectlyBouncyPuddle({ 6, 0.0, 2 }, true);

	// MARK: Cubes
    bool isSimulated = false;
	SharedScene::createCube({ 6.0f, 1.0f, 12.0f }, false);
	SharedScene::createCube({ 3.0f, 2.0f, 10.0f }, false);
	SharedScene::createCube({ -3.0f, 1.0f, -9.0f }, false);
	SharedScene::createCube({ -6.0f, 1.0f, -8.0f }, false);

	// MARK: Portals
    const glm::vec3 portalScale = glm::vec3(1.6f, 0.01f, 2.8f);
	glm::quat rotationY1 = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
	rotationY1 = glm::angleAxis(glm::radians(-45.0f), glm::vec3(0, 0, 1)) * rotationY1;

	glm::quat rotationY2 = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
	rotationY2 = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0)) * rotationY2;

	const glm::vec3 pos1 = glm::vec3(4.0f, 1.5f, -5.0f);
	const glm::vec3 pos2 = glm::vec3(-1.0f, 1.5f, -8.0f);

	auto bluePortal = ObjectBuilder::construct<LActor>().lock();
	bluePortal->loadComponent<LG::BluePortal>([pos1](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::BluePortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos1, pos1 + cameraFront, cameraUp));

			auto p1View = glm::lookAt(pos1, pos1 + cameraFront, cameraUp);

			auto playerView = LRenderer::get()->getView();

			auto modifiedModel = p1View * playerView;

			return;
		});
	bluePortal->loadComponent<LP::RigidBody>([pos1, portalScale, rotationY1](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			auto player = LPlayerCharacter::get();
			player->bluePortal = physicsComponent;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = pos1;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY1;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::CollisionManifold collision, float dt) {};
		});

	auto orangePortal = ObjectBuilder::construct<LActor>().lock();
	orangePortal->loadComponent<LG::OrangePortal>([pos2](LG::LGraphicsComponent* graphicsComponent)
		{
			const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			dynamic_cast<LG::OrangePortal*>(graphicsComponent)->setPortalView(glm::lookAt(pos2, pos2 + cameraFront, cameraUp));
		});
	orangePortal->loadComponent<LP::RigidBody>([pos2, portalScale, rotationY2](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			auto player = LPlayerCharacter::get();
			player->orangePortal = physicsComponent;

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = pos2;
			physicsComponent->transform.scale = portalScale;
			physicsComponent->transform.rotation *= rotationY2;
			physicsComponent->isTrigger = true;

			physicsComponent->onCollision = [player](LP::CollisionManifold collision, float dt) {};
		});
}