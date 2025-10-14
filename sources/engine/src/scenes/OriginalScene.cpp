#include "scenes/Shared.h"
#include "scenes/OriginalScene.h"
#include "LEngine.h"

void OriginalScene::createFloor(float scale) 
{
	auto floor = ObjectBuilder::construct<LActor>().lock();
	floor->loadComponent<LG::LPlane>();
	floor->loadComponent<LP::RigidBody>([scale](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);

			physicsComponent->collider = planeYUPCollider;
			physicsComponent->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
			physicsComponent->transform.scale = glm::vec3(scale, 1.0f, scale);
			physicsComponent->material = LP::Material::Ice;
		});
	floor->graphicsComponent->setColorTexture("textures/smile.jpg");
}

void OriginalScene::createOriginalSample(bool isSimulated)
{

	auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
	cubeA->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 1.0f;
			physicsComponent->transform.position = glm::vec3(2.0f, 1.0f, 0.0f);
		});

	auto cubeB = ObjectBuilder::construct<LActor>().lock();
	cubeB->loadComponent<LG::LCube>();
	cubeB->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->material.restitution = 2.2f;
			physicsComponent->transform.position = glm::vec3(2.0f, 3.0f, 0.0f);
		});

	auto cubeC = ObjectBuilder::construct<LActor>().lock();
	cubeC->loadComponent<LG::LCube>();
	cubeC->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(isSimulated);
			if (isSimulated) physicsComponent->setMass(10.0f);

			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Rubber;
			physicsComponent->transform.position = glm::vec3(2.0f, 5.0f, 0.0f);
		});
}

void OriginalScene::createStairs(int height, int maxLength)
{
    int directionIndex = 0;
    int stepCount = 0;
    int currentLength = 0;
    glm::ivec2 position(0, 0); // Starting position

    // Movement directions: forward, right, backward, left
    glm::ivec2 directions[4] = 
	{
        {0, 1},   // forward (up)
        {1, 0},   // right
        {0, -1},  // backward (down)
        {-1, 0}   // left
    };

    for (int i = 0; i < height; ++i)
    {
		auto step = ObjectBuilder::construct<LActor>().lock();
		step->loadComponent<LG::LCube>().
			loadComponent<LP::RigidBody>([position, i](LP::RigidBody* physicsComponent)
			{
				physicsComponent->setIsSimulated(false);
				
				physicsComponent->collider = cubeOBBCollider;
				physicsComponent->transform.position = glm::vec3((float)position.x * 0.85f, (float)i, (float)position.y * 0.85f);
			});

		if (i % 2 == 0)
		{
			step->graphicsComponent->setColorTexture("textures/smile.jpg");
		}
		else
		{
			step->graphicsComponent->setColorTexture("textures/smile2.jpg");
		}

        // Move to the next position
        position += directions[directionIndex];
        ++currentLength;

        // Change direction if reached max length
        if (currentLength >= maxLength)
        {
            currentLength = 0;
            directionIndex = (directionIndex + 1) % 4;  // Cycle through 0-3
        }
    }
}

void OriginalScene::createPerfectlyBouncyPuddle()
{
	auto puddle = ObjectBuilder::construct<LActor>().lock();
	puddle->loadComponent<LG::LCube>();
	puddle->loadComponent<LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			
			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = glm::vec3(7.0f, 0.5f, 7.0f);
			physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
			// Must be adjusted to accomodate the material of the object that wishes to bounce:
			// bouncyPuddle.restituion = 1 / object.restituion
			physicsComponent->material.restitution = 2.0; 
		});
	puddle->graphicsComponent->setColorTexture("textures/smile.jpg");
}

void OriginalScene::createStairsStressTest(int height, int maxLength, float YStep)
{
    int directionIndex = 0;
    int stepCount = 0;
    int currentLength = 0;
    glm::ivec2 position(0, 0); // Starting position

    // Movement directions: forward, right, backward, left
    glm::ivec2 directions[4] = 
	{
        {0, 1},   // forward (up)
        {1, 0},   // right
        {0, -1},  // backward (down)
        {-1, 0}   // left
    };

    for (int i = -maxLength; i < height - maxLength; ++i)
    {
		auto step = ObjectBuilder::construct<LActor>().lock();
		step->loadComponent<LG::LCube>();
		step->loadComponent<LP::RigidBody>([position, i, YStep](LP::RigidBody* physicsComponent)
			{
				physicsComponent->setIsSimulated(false);
				
				physicsComponent->collider = cubeOBBCollider;
				physicsComponent->transform.position = glm::vec3((float)position.x * 0.85f, (float)i * YStep, (float)position.y * 0.85f);
			});

        // Move to the next position
        position += directions[directionIndex];
        ++currentLength;

        // Change direction if reached max length
        if (currentLength >= maxLength)
        {
            currentLength = 0;
            directionIndex = (directionIndex + 1) % 4;  // Cycle through 0-3
        }

		step->graphicsComponent->setColorTexture("textures/smile.jpg");
    }
}

void OriginalScene::createStairsStressTest2(int height, int maxLength, float YStep)
{
    int directionIndex = 0;
    int stepCount = 0;
    int currentLength = 0;
    glm::ivec2 position(0, 0); // Starting position

    // Movement directions: forward, right, backward, left
    glm::ivec2 directions[4] = 
	{
        {0, 1},   // forward (up)
        {1, 0},   // right
        {0, -1},  // backward (down)
        {-1, 0}   // left
    };

    for (int i = -maxLength; i < height - maxLength; ++i)
    {
		auto step = ObjectBuilder::construct<LActor>().lock();
		step->loadComponent<LG::LCube>();
		step->loadComponent<LP::RigidBody>([position, i, YStep](LP::RigidBody* physicsComponent)
			{
				physicsComponent->setIsSimulated(false);
				
				physicsComponent->collider = cubeOBBCollider;
				physicsComponent->transform.position = glm::vec3((float)position.x, (float)i * YStep, (float)position.y);
			});

        // Move to the next position
        position += directions[directionIndex];
        ++currentLength;

        // Change direction if reached max length
        if (currentLength >= maxLength)
        {
            currentLength = 0;
            directionIndex = (directionIndex + 1) % 4;  // Cycle through 0-3
        }
    }
}

void OriginalScene::createPerfectlyBouncyPuddleNearHeavenlyOrbit(int height, int maxLength, float YStep)
{
	auto puddle = ObjectBuilder::construct<LActor>().lock();
	puddle->loadComponent<LG::LCube>();
	puddle->loadComponent<LP::RigidBody>([height, maxLength, YStep](LP::RigidBody* physicsComponent)
		{
			physicsComponent->setIsSimulated(false);
			
			physicsComponent->collider = cubeOBBCollider;
			physicsComponent->transform.position = glm::vec3(7.0f, (height - maxLength) * YStep, 7.0f);
			physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
			// Must be adjusted to accomodate the material of the object that wishes to bounce:
			// bouncyPuddle.restituion = 1 / object.restituion
			physicsComponent->material.restitution = 5.0; 
		});
}