#include <LWindow.h>
#include "LEngine.h"
#include "LPlayerCharacter.h"
#include <collision/AABBCollider.h>

// MARK: - Shared Colliders
auto cubeAABBCollider = std::make_shared<LP::AABBCollider>(LP::AABBCollider::makeCube());

void createPlayer() 
{
	auto weakPlayer = ObjectBuilder::construct<LPlayerCharacter>();
	auto playerCharacter = weakPlayer.lock();
	playerCharacter->loadComponent<LP::RigidBody>([weakPlayer](LP::RigidBody* physicsComponent)
		{
			physicsComponent->m_takesGravity = true;
			physicsComponent->m_isSimulated = true;

			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->transform.position = glm::vec3(2.0f, 2.0f, 2.0f);
			physicsComponent->transform.scale = LPlayerCharacter::standingDimensions;
			physicsComponent->m_mass = LPlayerCharacter::mass;
			physicsComponent->m_onCollision = [weakPlayer](LP::Collision collision, float depth) {
				if (collision.points.normal.y > 0)
				{
					auto playerCharacter = weakPlayer.lock();
					playerCharacter->resetJump();
				}
			};
		});
}

void createFloor(float scale = 20.0f) 
{
	auto floor = ObjectBuilder::construct<LActor>().lock();
	floor->loadComponent<LG::LPlane>();
	floor->loadComponent<LP::RigidBody>([scale](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = false;
			physicsComponent->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
			physicsComponent->transform.scale = glm::vec3(scale, 1.0f, scale);
			physicsComponent->m_mass = 100000;
			physicsComponent->material = LP::Material::Ice;
		});
	floor->graphicsComponent->setColorTexture("textures/smile.jpg");
}

void createOriginalSample(bool isSimulated = false)
{

	auto cubeA = ObjectBuilder::construct<LActor>().lock();
	cubeA->loadComponent<LG::LCube>();
	cubeA->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = isSimulated;
			physicsComponent->m_takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->transform.position = glm::vec3(0.0f, 1.0f, 0.0f);
			physicsComponent->m_mass = 10.0f;
		});

	auto cubeB = ObjectBuilder::construct<LActor>().lock();
	cubeB->loadComponent<LG::LCube>();
	cubeB->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = isSimulated;
			physicsComponent->m_takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->transform.position = glm::vec3(1.0f, 2.0f, 0.0f);
			physicsComponent->m_mass = 10.0f;
		});

	auto cubeC = ObjectBuilder::construct<LActor>().lock();
	cubeC->loadComponent<LG::LCube>();
	cubeC->loadComponent< LP::RigidBody>([isSimulated](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = isSimulated;
			physicsComponent->m_takesGravity = isSimulated;
			physicsComponent->material = LP::Material::Metal;
			physicsComponent->transform.position = glm::vec3(2.0f, 3.0f, 0.0f);
			physicsComponent->m_mass = 10.0f;
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
		step->loadComponent<LG::LCube>().
			loadComponent<LP::RigidBody>([position, i](LP::RigidBody* physicsComponent)
			{
				physicsComponent->collider = cubeAABBCollider;
				physicsComponent->m_isSimulated = false;
				physicsComponent->transform.position = glm::vec3((float)position.x * 0.85f, (float)i, (float)position.y * 0.85f);
				physicsComponent->m_mass = 10.0f;
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

void createPerfectlyBouncyPuddle()
{
	auto puddle = ObjectBuilder::construct<LActor>().lock();
	puddle->loadComponent<LG::LCube>();
	puddle->loadComponent<LP::RigidBody>([](LP::RigidBody* physicsComponent)
		{
			physicsComponent->collider = cubeAABBCollider;
			physicsComponent->m_isSimulated = false;
			physicsComponent->transform.position = glm::vec3(7.0f, 0.5f, 7.0f);
			physicsComponent->transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
			// Must be adjusted to accomodate the material of the object that wishes to bounce:
			// bouncyPuddle.restituion = 1 / object.restituion
			physicsComponent->material.restitution = 2.5; 
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
				physicsComponent->m_mass = 10.0f;
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
	// createOriginalSample(true);
	createStairs(100);
	// createStairsStressTest(60000, 40, 0.001f);
	createPerfectlyBouncyPuddle();
	
	// MARK: RunLoop
	engine.loop();
}