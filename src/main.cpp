#include <LWindow.h>
#include "LEngine.h"
#include <collision/AABBCollider.h>

// MARK: - Shared Colliders
auto cubeAABBCollider = std::make_shared<LatropPhysics::AABBCollider>(LatropPhysics::AABBCollider::makeCube());

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LWindow wnd(wndSpecs);

	LEngine engine(wnd);

	auto character = ObjectBuilder::construct(std::make_shared<PlayerCharacter>(
		nullptr, 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, true, true))
	);
	auto characterPhyscis = character.lock()->physicsComponent;
	characterPhyscis->transform.position = glm::vec3(0.0f, 0.0f, 10.0f);

	auto floor = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, false))
	);
	auto bodyFloor = floor.lock()->physicsComponent;
	bodyFloor->transform.position = glm::vec3(0.0f, -2.0f, 0.0f);
	bodyFloor->transform.scale = glm::vec3(1000.0f, 1.0f, 1000.0f);
	bodyFloor->m_restitution = 1;
	bodyFloor->m_mass = 100000;

	auto cubeA = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, true, true))
	);
	auto bodyA = cubeA.lock()->physicsComponent;
	bodyA->transform.position = glm::vec3(0.0f, 8.0f, 0.0f);
	bodyA->m_restitution = 1;
	bodyA->m_mass = 1;

	auto cubeB = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, true, true))
	);
	auto bodyB = cubeB.lock()->physicsComponent;
	bodyB->transform.position = glm::vec3(0.0f, 15.0f, 0.0f);
	bodyB->m_restitution = 1;
	bodyB->m_mass = 1;

	auto cubeC = ObjectBuilder::construct(std::make_shared<LActor>(
		std::make_shared<LG::LCube>(), 
		std::make_shared<LatropPhysics::RigidBody>(cubeAABBCollider, true, true))
	);
	auto bodyC = cubeC.lock()->physicsComponent;
	bodyC->transform.position = glm::vec3(5.0f, 0.0f, 0.0f);
	bodyC->m_restitution = 0.5;
	bodyC->m_mass = 0.95;
	
	engine.loop();
}