#pragma once

#include <functional>
#include <dynamics/DynamicsWorld.h>
#include <LRenderer.h>

#include "LActor.h"
#include "LTickable.h"
#include "TraceableDynamicsWorld.h"

class LEngine
{
public:

	LEngine(std::unique_ptr <LWindow> window);
	static LEngine* get() { return thisPtr; }

	void beginPlay();
	void endPlay();

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
	LatropPhysics::TraceableDynamicsWorld physicsWorld;
	std::vector<std::shared_ptr<LActor>> objects;

protected:

	void executeTickables();

	static LEngine* thisPtr;
	std::unique_ptr<LRenderer> renderer;
	std::unique_ptr<LWindow> window;

	std::vector<std::weak_ptr<LTickable>> tickables;

	std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

	float fpsTimer = 0.0f;
	uint32 fps = 0;
};

class ObjectBuilder
{
public:

	template<typename Actor>
	[[nodiscard]] static std::weak_ptr<Actor> construct()
	{
		auto object = std::make_shared<Actor>();
		LEngine::get()->objects.push_back(object);

		if (auto tickable = std::dynamic_pointer_cast<LTickable>(object))
		{
			LEngine::get()->addTickablePrimitive(tickable);
		}

		return object;
	}

	template<typename Component>
	[[nodiscard]] static void registerComponent(std::shared_ptr<Component> component)
	{
		if constexpr (std::is_base_of<LG::LGraphicsComponent, Component>::value)
		{
			RenderComponentBuilder::adjustImpl(component);
		}

		else if constexpr (std::is_base_of<LatropPhysics::CollisionBody, Component>::value)
		{
			LEngine::get()->physicsWorld.addRigidBody(component);
		}
	}
};