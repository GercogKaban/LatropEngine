#pragma once

#include <functional>
#include <dynamics/DynamicsWorld.h>
#include <LRenderer.h>

#include "LActor.h"
#include "LTickable.h"
#include "TraceableDynamicsWorld.h"

class LEngine
{
	friend class ObjectBuilder;

public:

	LEngine(std::unique_ptr <LWindow> window);
	static LEngine* get() { return thisPtr; }

	bool isGameStarted() const { return bGameStarted; }

	void loop();

	float getDelta() const
	{
		return std::chrono::duration<float>(currentFrameTime - previousFrameTime).count();
	}

protected:

	void beginPlay();
	void endPlay();
	void initObjects();

	void updateDelta()
	{
		previousFrameTime = currentFrameTime;
		currentFrameTime = std::chrono::high_resolution_clock::now();
	}

	void addTickablePrimitive(std::weak_ptr<LTickable> ptr);
	void executeTickables();
	void addObjectToInit(std::shared_ptr<LActor> objectToInit);

	static LEngine* thisPtr;
	std::unique_ptr<LRenderer> renderer;
	std::unique_ptr<LWindow> window;

	LP::TraceableDynamicsWorld physicsWorld;

	std::vector<std::shared_ptr<LActor>> objects;
	std::vector<std::shared_ptr<LActor>> objectsToInit;

	std::vector<std::weak_ptr<LTickable>> tickables;

	std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

	const float physicsIterationsCount = 128.0;
	float fpsTimer = 0.0f;
	uint32 fps = 0;

	bool bGameStarted = false;
};

class ObjectBuilder
{
public:

	template<typename Actor>
	[[nodiscard]] static std::weak_ptr<Actor> construct()
	{
		auto object = std::make_shared<Actor>();

		if (LEngine* engine = LEngine::get())
		{	
			engine->objectsToInit.push_back(object);
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

		else if constexpr (std::is_base_of<LP::CollisionBody, Component>::value)
		{
			LEngine::get()->physicsWorld.addRigidBody(component);
		}
	}
};