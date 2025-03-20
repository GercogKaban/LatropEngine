#include "LEngine.h"
#include <glfw3.h>
#include <dynamics/ImpulseSolver.h>
#include <dynamics/PositionSolver.h>
#include "../_deps/tracy-src/public/tracy/Tracy.hpp"

LEngine* LEngine::thisPtr = nullptr;

LEngine::LEngine(std::unique_ptr<LWindow> window)
	:window(std::move(window))
{
	thisPtr = this;
	physicsWorld.addSolver(std::make_unique<LatropPhysics::ImpulseSolver>());
	physicsWorld.addSolver(std::make_unique<LatropPhysics::PositionSolver>());
}

void LEngine::beginPlay()
{
	for (auto object : objects)
	{
		object->beginPlay();
	}
}

void LEngine::endPlay()
{
	for (auto object : objects)
	{
		object->endPlay();
	}
	objects.clear();
}

void LEngine::addTickablePrimitive(std::weak_ptr<LTickable> ptr)
{
	tickables.push_back(ptr);
}

void LEngine::loop()
{
	renderer = std::make_unique<LRenderer>(window);

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

		executeTickables();
		glfwPollEvents();

		if (renderer->bNeedToUpdateProjView)
		{
			renderer->updateProjView();
		}

		{
			ZoneScopedN("Physics iterations");
			auto miniDelta = getDelta() / 8.0f;
			for (int i = 0; i < 8; i++)
			{
				physicsWorld.integrate(miniDelta);
			}
		}

		{
			ZoneScopedN("Draw");
			renderer->drawFrame();
		}
		fps++;
	}
	vkDeviceWaitIdle(renderer->logicalDevice);
	endPlay();
}

void LEngine::executeTickables()
{
	ZoneScoped;
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
