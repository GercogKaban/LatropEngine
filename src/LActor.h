#pragma once

#include <Primitives.h>
#include <dynamics/RigidBody.h>

// represents a physical object with visualization

class LActor
{
public:

	friend class ObjectBuilder;
	friend class LEngine;

	LActor()
	{
	}

	template <typename Component>
	void loadComponent(std::function<void(Component*)>&& initLogics = [](Component*){})
	{
		// graphics component
		if constexpr (std::is_base_of<LG::LGraphicsComponent, Component>::value)
		{
			auto graphicsComponentReal = std::make_shared<Component>();
			graphicsComponent = graphicsComponentReal;
			initGraphicsComponent = [initLogics, graphicsComponentReal]() {initLogics(graphicsComponentReal.get()); };
		}

		// physics component
		else if constexpr (std::is_base_of<LatropPhysics::CollisionBody, Component>::value)
		{
			auto physicsComponentReal = std::make_shared<Component>();
			physicsComponent = physicsComponentReal;
			initPhysicsComponent = [initLogics, physicsComponentReal](){initLogics(physicsComponentReal.get()); };
		}
	}

	virtual ~LActor() = default;

	std::shared_ptr<LG::LGraphicsComponent> graphicsComponent;
	std::shared_ptr<LatropPhysics::RigidBody> physicsComponent;

protected:

	virtual void beginPlay();
	virtual void endPlay(){}

	std::function<void()> initGraphicsComponent;
	std::function<void()> initPhysicsComponent;

	// DEBUG_CODE(std::shared_ptr<LG::LGraphicsComponent> debugRenderComponent;)
};

