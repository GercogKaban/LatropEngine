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

	inline std::chrono::duration<float> getDelta() const
	{
		return frameTimeCurrent - frameTimePrevious;
	}

	inline std::chrono::duration<float> getFixedDelta() const
	{
		return fixedDelta;
	}

protected:

	void beginPlay();
	void endPlay();
	void initObjects();

	void updateDelta()
	{
		frameTimePrevious = frameTimeCurrent;
		frameTimeCurrent = std::chrono::high_resolution_clock::now();
	}

	void addTickablePrimitive(std::weak_ptr<LTickable> ptr);
	void executeTickables(float dt);
	void addObjectToInit(std::shared_ptr<LActor> objectToInit);

	static LEngine* thisPtr;
	std::unique_ptr<LRenderer> renderer;
	std::unique_ptr<LWindow> window;

	LP::TraceableDynamicsWorld physicsWorld;

	std::vector<std::shared_ptr<LActor>> objects;
	std::vector<std::shared_ptr<LActor>> objectsToInit;

	std::vector<std::weak_ptr<LTickable>> tickables;

	std::chrono::high_resolution_clock::time_point frameTimeCurrent = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point frameTimePrevious = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> frameTimeAccumulator = std::chrono::duration<float>(0.0f);

	const std::chrono::duration<float> fixedDelta = std::chrono::duration<float>(0.001);
	const float physicsCellSize = 1.5f;
	bool requiresPhysicsGridUpdate = true;

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