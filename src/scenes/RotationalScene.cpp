#include "scenes/Shared.h"
#include "scenes/RotationalScene.h"
#include "LEngine.h"

void RotationalScene::createScenarioI()
{
    SharedScene::createPlayer({ 0.0f, 0.0f, 4.0f }, false);

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
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->transform.position = glm::vec3(0.6f, 5.0f, 0.0f);
		});
}