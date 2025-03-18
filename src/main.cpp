#include <LWindow.h>
#include "LEngine.h"
#include <collision/AABBCollider.h>

// MARK: - Shared Colliders
auto cubeAABBCollider = std::make_shared<LatropPhysics::AABBCollider>(LatropPhysics::AABBCollider::makeCube());

void createPlayer() 
{
	auto character = ObjectBuilder::construct(std::make_shared<PlayerCharacter>(
		nullptr, 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, true, true), glm::vec3(2.0f, 2.0f, 2.0f))
	);
	auto characterPhyscis = character.lock()->physicsComponent;
	characterPhyscis->transform.scale = glm::vec3(0.55, 1.0f, 0.55f);
}

void createFloor() 
{
	auto floor = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, false)
	));
	auto bodyFloor = floor.lock()->physicsComponent;
	bodyFloor->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	bodyFloor->transform.scale = glm::vec3(20, 1.0f, 20);
	bodyFloor->m_restitution = 0.0;
	bodyFloor->m_mass = 100000;
}

void createOriginalSample()
{
	auto cubeA = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, false, false)
	));
	auto bodyA = cubeA.lock()->physicsComponent;
	bodyA->transform.position = glm::vec3(0.0f, 1.0f, 0.0f);
	bodyA->m_restitution = 0;
	bodyA->m_mass = 1;

	auto cubeB = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, false, false)
	));
	auto bodyB = cubeB.lock()->physicsComponent;
	bodyB->transform.position = glm::vec3(1.0f, 2.0f, 0.0f);
	bodyB->m_restitution = 0.0;
	bodyB->m_mass = 1;

	auto cubeC = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, false, false)
	));
	auto bodyC = cubeC.lock()->physicsComponent;
	bodyC->transform.position = glm::vec3(2.0f, 3.0f, 0.0f);
	bodyC->m_restitution = 0.0;
	bodyC->m_mass = 1;
}

void createStairs(int x, int y, int maxLength = 3)
{
    int directionIndex = 0;
    int stepCount = 0;
    int currentLength = 0;
    glm::ivec2 position(0, 0); // Starting position

    // Movement directions: forward, right, backward, left
    glm::ivec2 directions[4] = {
        {0, 1},   // forward (up)
        {1, 0},   // right
        {0, -1},  // backward (down)
        {-1, 0}   // left
    };

    for (int i = 0; i < x * y; ++i)
    {
        auto step = ObjectBuilder::construct(std::make_shared<LActor>(
            std::make_shared<LG::LCube>(), 
            std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, false, false)
		));
        auto stepBody = step.lock()->physicsComponent;

        // Set position
        stepBody->transform.position = glm::vec3((float)position.x * 0.85, (float)i, (float)position.y * 0.85);
        stepBody->m_restitution = 0.0;

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
	LWindow wnd(wndSpecs);

	LEngine engine(wnd);

	// MARK: Samples
	createPlayer();
	createFloor();
	createStairs(10, 10);
	
	// MARK: RunLoop
	engine.loop();
}