#include <LWindow.h>
#include "LEngine.h"
#include "LPlayerCharacter.h"
#include <collision/AABBCollider.h>

// MARK: - Shared Colliders
auto cubeAABBCollider = std::make_shared<LP::AABBCollider>(LP::AABBCollider::makeCube());

void createPlayer() 
{
	auto playerCharacter = ObjectBuilder::construct<LPlayerCharacter>().lock();
	playerCharacter->loadComponent<LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->m_takesGravity = true;
			physicsComponent->m_isSimulated = true;

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->transform.position = glm::vec3(2.0f, 2.0f, 2.0f);
			physicsComponent->transform.scale = glm::vec3(0.55, 1.0f, 0.55f);
		});
}

void createFloor() 
{
	auto floor = ObjectBuilder::construct<LActor>().lock();
	floor->loadComponent<LG::LCube>();
	floor->loadComponent<LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = false;
			physicsComponent->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
			physicsComponent->transform.scale = glm::vec3(20, 1.0f, 20);
			physicsComponent->m_restitution = 0.0;
			physicsComponent->m_mass = 100000;
		});
}

void createOriginalSample()
{

	auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
	cubeA->loadComponent< LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = false;
			physicsComponent->transform.position = glm::vec3(0.0f, 1.0f, 0.0f);
			physicsComponent->m_restitution = 0;
			physicsComponent->m_mass = 1;
		});

	auto cubeB = ObjectBuilder::construct<LActor>().lock();
	cubeB->loadComponent<LG::LCube>();
	cubeB->loadComponent< LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = false;
			physicsComponent->transform.position = glm::vec3(1.0f, 2.0f, 0.0f);
			physicsComponent->m_restitution = 0.0f;
			physicsComponent->m_mass = 1.0f;
		});

	auto cubeC = ObjectBuilder::construct<LActor>().lock();
	cubeC->loadComponent<LG::LCube>();
	cubeC->loadComponent< LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = false;
			physicsComponent->transform.position = glm::vec3(2.0f, 3.0f, 0.0f);
			physicsComponent->m_restitution = 0.0f;
			physicsComponent->m_mass = 1.0f;
		});
}

void createStairs(int height, int maxLength = 3)
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
		step->loadComponent<LG::LCube>();
		step->loadComponent<LP::RigidBody>([position, i](LP::RigidBody* physicsComponent)
			{
				physicsComponent->collider = cubeAABBCollider;
				physicsComponent->m_isSimulated = false;
				physicsComponent->transform.position = glm::vec3((float)position.x * 0.85f, (float)i, (float)position.y * 0.85f);
				physicsComponent->m_restitution = 0.0f;
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

void createBouncyPuddle()
{
	auto puddle = ObjectBuilder::construct<LActor>().lock();
	puddle->loadComponent<LG::LCube>();
	puddle->loadComponent<LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = false;
			physicsComponent->transform.position = glm::vec3(7.0f, 0.015f, 7.0f);
			physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
			physicsComponent->m_restitution = 1.0f;
			physicsComponent->m_mass = 100000.0f;
		});
}

void createStairsStressTest(int height, int maxLength = 3, float YStep = 0.01f)
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
				physicsComponent->collider = cubeAABBCollider;
				physicsComponent->m_isSimulated = false;
				physicsComponent->transform.position = glm::vec3((float)position.x * 0.85f, (float)i * YStep, (float)position.y * 0.85f);
				physicsComponent->m_restitution = 0.0f;
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

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LEngine engine(std::make_unique<LWindow>(wndSpecs));

	// MARK: Samples
	createPlayer();
	createFloor();
	// createStairs(100);
	createStairsStressTest(60000, 40, 0.001f);
	createBouncyPuddle();
	
	// MARK: RunLoop
	engine.loop();
}