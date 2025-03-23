#include "LEngine.h"
#include <glfw3.h>
#include <dynamics/ImpulseSolver.h>
#include <dynamics/PositionSolver.h>
#include <tracy/Tracy.hpp>

LEngine* LEngine::thisPtr = nullptr;

LEngine::LEngine(std::unique_ptr<LWindow> window)
	:window(std::move(window))
{
	thisPtr = this;
	physicsWorld.addSolver(std::make_unique<LP::ImpulseSolver>());
	physicsWorld.addSolver(std::make_unique<LP::PositionSolver>());
}

void LEngine::beginPlay()
{
	bGameStarted = true;
}

void LEngine::endPlay()
{
	for (auto object : objects)
	{
		object->endPlay();
	}
	objects.clear();
}

void LEngine::initObjects()
{
	ZoneScopedN("Pass: Init objects");
	objects.reserve(objects.size() + objectsToInit.size());
	for (auto object : objectsToInit)
	{
		object->beginPlay();
		objects.push_back(object);
		if (auto tickable = std::dynamic_pointer_cast<LTickable>(object))
		{
			addTickablePrimitive(tickable);
		}
	}

	physicsWorld.updateSpacialPartitioningOfStaticBodies(10.0f);
	objectsToInit.clear();
}

void LEngine::addTickablePrimitive(std::weak_ptr<LTickable> ptr)
{
	tickables.push_back(ptr);
}

void LEngine::loop()
{
	renderer = std::make_unique<LRenderer>(window, LActor::getComponentCounter());

	beginPlay();

	while (renderer->window && !glfwWindowShouldClose(renderer->window))
	{
		FrameMark;
		updateDelta();
		fpsTimer += getDelta();
		if (fpsTimer >= 1.0f)
		{
			fpsTimer = 0.0f;
			fps = 0;
		}

		initObjects();
		
		glfwPollEvents();
		executeTickables();

		if (renderer->bNeedToUpdateProjView)
		{
			renderer->updateProjView();
		}

		{
			ZoneScopedNC("Pass: Physics", 0xFF00AACC);
			auto miniDelta = getDelta() / physicsIterationsCount;
			for (int i = 0; i < physicsIterationsCount; i++)
			{
				physicsWorld.integrate(miniDelta);
			}
		}

		{
			ZoneScopedNC("Pass: Rendering", 0xFFFF007F);
			renderer->drawFrame();
		}
		fps++;
	}
	vkDeviceWaitIdle(renderer->logicalDevice);
	endPlay();
}

void LEngine::executeTickables()
{
	ZoneScopedNC("Pass: Tickables", 0xFF88CC00);
	for (auto it = tickables.begin(); it != tickables.end(); ++it)
	{
		if (!it->expired())
		{
			dynamic_cast<LTickable*>(it->lock().get())->tick(getDelta());
		}
		else
		{
			it = tickables.erase(it);
		}
	}
}

void LEngine::addObjectToInit(std::shared_ptr<LActor> objectToInit)
{
	objectsToInit.push_back(objectToInit);
}
