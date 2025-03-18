#pragma once

#include <functional>
#include <dynamics/DynamicsWorld.h>
#include <LRenderer.h>

#include "LCore.h"

class LTickable
{
public:

	LTickable() = default;
	virtual ~LTickable() = default;

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

	PlayerCharacter(
		std::shared_ptr<LG::LGraphicsComponent> graphicsComponent, 
		std::shared_ptr<LatropPhysics::RigidBody> physicsComponent,
		const glm::vec3& startPosition);

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
};

class ObjectBuilder
{
public:

	template<typename DebugRenderComponent = LG::LDummy>
	[[nodiscard]] static std::weak_ptr<LActor> construct(std::shared_ptr<LActor> object)
	{
		LEngine::get()->objects.push_back(object);

		if (auto tickable = std::dynamic_pointer_cast<LTickable>(object))
		{
			LEngine::get()->addTickablePrimitive(tickable);
		}

		// Render

		if (object->graphicsComponent)
		{
			RenderComponentBuilder::adjustImpl(object->graphicsComponent);
			LRenderer::get()->addPrimitve(object->graphicsComponent);
		}

		// Physics
		if (object->physicsComponent)
		{
			LEngine::get()->physicsWorld.addRigidBody(object->physicsComponent);
		}

		//DEBUG_CODE(
		//if constexpr (std::is_base_of<LG::LGraphicsComponent, DebugRenderComponent>::value)
		//{
		//	object->debugrenderComponent = RenderComponentBuilder::constructDebug<DebugRenderComponent>();
		//})

		return object;
	}
};