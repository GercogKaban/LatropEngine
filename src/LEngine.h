#pragma once

#include <functional>
#include <dynamics/DynamicsWorld.h>
#include <LRenderer.h>

#include "LCore.h"

class LTickable
{
public:

	LTickable() = default;
	~LTickable() = default;

	virtual void tick(float delta) = 0;
};

class LEngine : public LRenderer
{
public:

	LEngine(const LWindow& window);
	static LEngine* get() { return thisPtr; }

	void addTickablePrimitive(std::weak_ptr<LTickable> ptr);

	void loop();

	void updateDelta()
	{
		previousFrameTime = currentFrameTime;
		currentFrameTime = std::chrono::high_resolution_clock::now();
	}

	float getDelta() const
	{
		return std::chrono::duration<float>(currentFrameTime - previousFrameTime).count();
	}

	// renderer
	LatropPhysics::DynamicsWorld physicsWorld;
	std::vector<std::shared_ptr<LActor>> objects;

protected:

	void executeTickables();

	std::vector<std::weak_ptr<LTickable>> tickables;

	std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

	float fpsTimer = 0.0f;
	uint32 fps = 0;
	static LEngine* thisPtr;
};

class PlayerCharacter : public LActor, public LTickable
{

public:

	PlayerCharacter();
	virtual void tick(float delta) override;

	float getSpeed() const
	{
		return speed;
	}

	static PlayerCharacter* get()
	{
		return thisPtr;
	}

protected:

	bool isKeyPressed(int32 keyCode) const;

	static void handleInput(class GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseInput(class GLFWwindow* window, double xpos, double ypos);

	void updateCamera(const glm::vec3& newLocation);

	std::unordered_map<int32, bool> pressedKeys;

	class LRenderer* renderer = nullptr;
	static PlayerCharacter* thisPtr;
	float speed = 1.0f;

	// should be in camera class
	bool bFirstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 600.0f;
	float lastY = 300.0f;
	float sensitivity = 0.1f;

	float centerX = 1920 / 2;
	float centerY = 1080 / 2;

	glm::vec3 location = glm::vec3(0.0f, 0.0f, 3.0f);
};

class ObjectBuilder
{
public:

	template<typename GameObject, typename PhysicsComponent = LG::LDummy, typename RenderComponent = LG::LDummy, typename DebugRenderComponent = LG::LDummy>
	[[nodiscard]] static std::weak_ptr<GameObject> construct()
	{
		DEBUG_CODE(bIsConstructing = true;)

		std::shared_ptr<GameObject> object = std::shared_ptr<GameObject>(new GameObject());
		LEngine::get()->objects.push_back(object);

		if constexpr (std::is_base_of<LTickable, GameObject>::value)
		{
			LEngine::get()->addTickablePrimitive(object);
		}

		if constexpr (!std::is_base_of<RenderComponent, LG::LDummy>::value)
		{
			object->renderComponent = RenderComponentBuilder::construct<RenderComponent>();
		}

		if constexpr (!std::is_base_of<PhysicsComponent, LG::LDummy>::value)
		{
			object->physicsComponent = std::shared_ptr<PhysicsComponent>(new PhysicsComponent());
			if (auto body = std::dynamic_pointer_cast<LatropPhysics::RigidBody>(object->physicsComponent))
			{
				LEngine::get()->physicsWorld.addRigidBody(body);
			} 
			else 
			{
				LEngine::get()->physicsWorld.addCollisionBody(object->physicsComponent);
			}
		}

		DEBUG_CODE(
		if constexpr (std::is_base_of<LG::LGraphicsComponent, DebugRenderComponent>::value)
		{
			object->debugrenderComponent = RenderComponentBuilder::constructDebug<DebugRenderComponent>();
		})

		DEBUG_CODE(bIsConstructing = false;)

		LEngine::get()->objects.push_back(object);

		return object;
	}

protected:

	template<typename GameObject>
	static void destruct(GameObject* object)
	{

	}

	DEBUG_CODE(
		static bool isConstructing() { return bIsConstructing; }
	)

protected:

	static std::unordered_map<std::string, int32> objectsCounter;

	DEBUG_CODE(
		static bool bIsConstructing;
	)
};