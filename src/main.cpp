#include <LWindow.h>
#include "LEngine.h"
#include <collision/AABBCollider.h>

class StaticCube : public LObject
{

};

int main()
{
	LWindow::LWindowSpecs wndSpecs{ LWindow::WindowMode::Windowed,"LGWindow",false, 1920, 1080 };
	LWindow wnd(wndSpecs);

	LEngine engine(wnd);

	auto character = ObjectBuilder::construct<PlayerCharacter>();
	// auto testcube = ObjectBuilder::construct<StaticCube, LG::LDummy, LG::LCube>();

	auto cubeA = ObjectBuilder::construct<StaticCube, LatropPhysics::CollisionBody, LG::LCube>();
	auto bodyA = std::dynamic_pointer_cast<LatropPhysics::CollisionBody>(cubeA->physicsBody);
	bodyA->transform = new LatropPhysics::Transform();
	bodyA->transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	bodyA->collider = new LatropPhysics::AABBCollider(
		{ -0.5f, -0.5f, -0.5f },
		{ 0.5f, 0.5f, 0.5f }
	);
	
	auto cubeB = ObjectBuilder::construct<StaticCube, LatropPhysics::RigidBody, LG::LCube>();
	auto bodyB = std::dynamic_pointer_cast<LatropPhysics::RigidBody>(cubeB->physicsBody);
	bodyB->m_takesGravity = true;
	bodyB->m_gravity = { 0, -1, 0 }; //LEngine::get()->physicsWorld.m_gravity;
	bodyB->m_mass = 1;
	bodyB->m_restitution = 2;
	bodyB->transform = new LatropPhysics::Transform();
	bodyB->transform->position = glm::vec3(0.5f, 10.0f, 0.0f);
	bodyB->transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	bodyB->collider = new LatropPhysics::AABBCollider(
		{ -0.5f, -0.5f, -0.5f },
		{ 0.5f, 0.5f, 0.5f }
	);
	
	engine.loop();
}