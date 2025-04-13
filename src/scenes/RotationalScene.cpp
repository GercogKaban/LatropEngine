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

// MARK: - Free Fall Scenarios

glm::vec3 freeFallOriginI   = { 0.9f, 3.0f, 0.0f };
glm::vec3 freeFallOriginII  = { 0.5f, 3.0f, 0.0f };
glm::vec3 freeFallOriginIII = { 0.3f, 3.0f, 0.0f };
glm::vec3 freeFallOriginIV  = { 0.155f, 3.0f, 0.0f };
glm::vec3 freeFallOriginV   = { 0.14f, 3.0f, 0.0f };

void cubeBFallsOnCubeAFrom(
    glm::vec3 origin, 
    glm::vec3 offset = glm::vec3(0.0f), 
    LP::Material aMaterial = LP::Material::Metal,
    bool bottomIsDynamic = false
) {
    auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
    cubeA->graphicsComponent->setColorTexture("textures/smile.jpg");

	cubeA->loadComponent<LP::RigidBody>([offset, aMaterial, bottomIsDynamic](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(bottomIsDynamic);
            // physicsComponent->transform.scale = { 2.0f, 1.0f, 2.0f };
            if (bottomIsDynamic) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = bottomIsDynamic;
			physicsComponent->material = aMaterial;
			physicsComponent->transform.position = offset;
		});

    auto cubeB = ObjectBuilder::construct<LActor>().lock();
	cubeB->loadComponent<LG::LCube>();
    cubeB->graphicsComponent->setColorTexture("textures/smile.jpg");

	cubeB->loadComponent<LP::RigidBody>([origin, offset](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(true);
            physicsComponent->setMass(0.100f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = true;
			physicsComponent->material = LP::Material::Wood;
			physicsComponent->transform.position = origin + offset;
		});
}

void RotationalScene::createFreeFallScenarioI()
{
    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);
    cubeBFallsOnCubeAFrom(freeFallOriginI);
}

void RotationalScene::createFreeFallScenarioII()
{
    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);
    cubeBFallsOnCubeAFrom(freeFallOriginII);
}

void RotationalScene::createFreeFallScenarioIII()
{
    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);
    cubeBFallsOnCubeAFrom(freeFallOriginIII);
}

void RotationalScene::createFreeFallScenarioIV()
{
    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);
    cubeBFallsOnCubeAFrom(freeFallOriginIV);
}

void RotationalScene::createFreeFallScenarioV()
{
    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);
    cubeBFallsOnCubeAFrom(freeFallOriginV);
}

void RotationalScene::createAllFreeFallScenarios()
{
    SharedScene::createPlayer({ 1.0f, 0.5f, 4.0f }, false);

    cubeBFallsOnCubeAFrom(freeFallOriginI,   { 0.0f, 0.0f, 0.0f });
    cubeBFallsOnCubeAFrom(freeFallOriginII,  { 0.0f, 0.0f, -2.0f });
    cubeBFallsOnCubeAFrom(freeFallOriginIII, { 0.0f, 0.0f, -4.0f });
    cubeBFallsOnCubeAFrom(freeFallOriginIV,  { 0.0f, 0.0f, -6.0f });
    cubeBFallsOnCubeAFrom(freeFallOriginV,   { 0.0f, 0.0f, -8.0f });
}

// MARK: - Bouncing Scenarios

void RotationalScene::createBouncingScenarioI()
{
    auto material = LP::Material::Metal;
    material.restitution = 1.0f;
    material.restitutionCombinator = LP::Material::CombinationMode::Maximum;

    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);
    cubeBFallsOnCubeAFrom({ 0.0f, 2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, material);
}

// MARK: - Stacking

void RotationalScene::createStackingScenarioI()
{
    SharedScene::createPlayer({ 0.0f, 0.5f, 4.0f }, false);
    cubeBFallsOnCubeAFrom({ 0.15f, 2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

    cubeBFallsOnCubeAFrom({ 0.f, 2.0f, 0.0f }, { 0.15f, 4.0f, 0.0f }, LP::Material::Metal, true);
}