#include "scenes/Shared.h"
#include "scenes/RotationalScene.h"
#include "LEngine.h"
#include <iostream>

void RotationalScene::createSanityCheck(bool isDiagonal)
{
    SharedScene::createPlayer({ 0.0f, 0.0f, 4.0f }, false);

    auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
    cubeA->graphicsComponent->setColorTexture("textures/smile.jpg");

	cubeA->loadComponent< LP::RigidBody>([isDiagonal](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(true);
            physicsComponent->setMass(10.0f);

			physicsComponent->takesGravity = false;
            physicsComponent->angularVelocity = glm::vec3(
                0.0f, 
                glm::pi<float>() / 2.0f, 
                isDiagonal ? glm::pi<float>() / 2.0f : 0.0f
            ); // Quarter turn per second
		});
}

void RotationalScene::createScenarioI()
{
    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);

    auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
    cubeA->graphicsComponent->setColorTexture("textures/smile.jpg");

	cubeA->loadComponent< LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = false;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
		});

    auto cubeB = ObjectBuilder::construct<LActor>().lock();
	cubeB->loadComponent<LG::LCube>();
    cubeB->graphicsComponent->setColorTexture("textures/smile.jpg");

	cubeB->loadComponent< LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(true);
            physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = true;
			physicsComponent->material = LP::Material::Wood;
			physicsComponent->transform.position = glm::vec3(0.9f, 3.0f, 0.0f);
            physicsComponent->onCollision = [](const LP::CollisionManifold& manifold, float dt) 
            {
                std::cout << "On Collision" << std::endl;

                for (uint8_t i = 0; i < manifold.contactsCount; ++i)
                {
                    const auto& location = manifold.contactPoints[i].location;
                    std::cout << "x: " << location.x << " y: " << location.y << " z: " << location.z << std::endl;
                }
            };
		});
}