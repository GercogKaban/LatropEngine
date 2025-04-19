#include "LEngine.h"
#include <glfw3.h>
#include <dynamics/RotationalImpulseSolver.h>
#include <dynamics/SmoothPositionSolver.h>
#include "LPlayerCharacter.h"

LEngine* LEngine::thisPtr = nullptr;

LEngine::LEngine(std::unique_ptr<LWindow> window)
	:window(std::move(window))
{
	thisPtr = this;
	physicsWorld.addSolver(std::make_unique<LP::RotationalImpulseSolver>());
	physicsWorld.addSolver(std::make_unique<LP::SmoothPositionSolver>());
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

	if (requiresPhysicsGridUpdate)
	{
		requiresPhysicsGridUpdate = false;
		physicsWorld.updateSpacialPartitioningOfStaticBodies(physicsCellSize);
	}
	objectsToInit.clear();
}

void LEngine::addTickablePrimitive(std::weak_ptr<LTickable> ptr)
{
	tickables.push_back(ptr);
}

void LEngine::loop()
{
	LRenderer::StaticInitData initData =
	{
		// TODO: hardcoded stuff should be changed
		.maxPortalNum = 2,
		.primitiveCounter = LActor::getComponentCounter(),
		.textures = LG::LGraphicsComponent::getInitTexturesData()
	};


	renderer = std::make_unique<LRenderer>(window, std::move(initData));
	beginPlay();

	while (renderer->getWindow() && !glfwWindowShouldClose(renderer->getWindow()))
	{
		FrameMark;

		std::chrono::duration dt = getDelta();
		updateDelta();
		frameTimeAccumulator += dt;

		fpsTimer += dt.count();
		if (fpsTimer >= 1.0f)
		{
			fpsTimer = 0.0f;
			fps = 0;
		}

		initObjects();

		
		glfwPollEvents();
		// Delta Update
		executeTickables(dt.count());

		// Fixed Delta Update
		while (frameTimeAccumulator >= fixedDelta)
		{
			ZoneScopedNC("Pass: Physics", 0xFF00AACC);

			physicsWorld.integrate(fixedDelta.count());
			frameTimeAccumulator -= fixedDelta;
		}
		
		renderer->playerModel = LPlayerCharacter::get()->physicsComponent->transform.getAsMatrix();
		renderer->playerOrientation = LPlayerCharacter::get()->orientation;

		{
			ZoneScopedNC("Pass: Rendering", 0x000000FF);
			renderer->drawFrame(dt.count());
		}
		fps++;
	}

	renderer->exit();
	endPlay();
}

void LEngine::executeTickables(float dt)
{
	ZoneScopedNC("Pass: Tickables", 0xFF88CC00);
	for (auto it = tickables.begin(); it != tickables.end(); ++it)
	{
		if (!it->expired())
		{
			dynamic_cast<LTickable*>(it->lock().get())->tick(dt);
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
