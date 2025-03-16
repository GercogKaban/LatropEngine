#include <LWindow.h>
#include "LEngine.h"
#include <collision/AABBCollider.h>

class StaticCube : public LActor
{
	// here we can 
};

// MARK: - Shared Colliders
LatropPhysics::AABBCollider cubeAABBCollider = LatropPhysics::CubeAABBCollider();


int main()
{
	//LatropPhysics::CubeAABBCollider cudeCollider;

	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LWindow wnd(wndSpecs);

	LEngine engine(wnd);

	auto character = ObjectBuilder::construct<PlayerCharacter>(new PlayerCharacter(
		new LG::LCube(),
		new LatropPhysics::RigidBody(false)
	));

	auto cubeA = ObjectBuilder::construct<LActor>(new LActor(
		new LG::LCube(),
		new LatropPhysics::RigidBody(false)
	));
	auto bodyA = cubeA.lock()->physicsComponent;
	bodyA->transform.position = glm::vec3(-0.0f, 0.0f, 0.0f);
	bodyA->collider = &cubeAABBCollider;

	// auto character = ObjectBuilder::construct<PlayerCharacter>(new PlayerCharacter());
	// auto testcube = ObjectBuilder::construct<StaticCube, LG::LDummy, LG::LCube>();

	// auto cubeA = ObjectBuilder::construct<StaticCube, LatropPhysics::CollisionBody, LG::LCube>();
	// // auto bodyA = std::dynamic_pointer_cast<LatropPhysics::CollisionBody>(cubeA.lock()->physicsComponent);
	// auto bodyA = cubeA.lock();
	// bodyA->transform.position = glm::vec3(-0.0f, 0.0f, 0.0f);
	// bodyA->collider = &cubeAABBCollider;
	
	// auto cubeB = ObjectBuilder::construct<StaticCube, LatropPhysics::RigidBody, LG::LCube>();
	// // auto bodyB = std::dynamic_pointer_cast<LatropPhysics::RigidBody>(cubeB.lock()->physicsComponent);
	// auto bodyB = cubeB.lock();
	// bodyB->m_takesGravity = true;
	// bodyB->m_gravity = LEngine::get()->physicsWorld.m_gravity;
	// bodyB->m_mass = 0.95;
	// bodyB->m_restitution = 2;
	// bodyB->transform.position = glm::vec3(0.5f, 10.0f, 0.0f);
	// bodyB->collider = &cubeAABBCollider;
	
	engine.loop();
}