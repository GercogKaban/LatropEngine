#pragma once

#include <Primitives.h>
#include <dynamics/RigidBody.h>
#include <unordered_map>

class LActor
{
public:

	friend class LEngine;

	LActor()
	{
	}

	template <typename Component>
	LActor& loadComponent(std::function<void(Component*)>&& initLogics = [](Component*){})
	{
		// graphics component
		if constexpr (std::is_base_of<LG::LGraphicsComponent, Component>::value)
		{
			auto graphicsComponentReal = std::make_shared<Component>();
			graphicsComponent = graphicsComponentReal;
			initGraphicsComponent = [initLogics, graphicsComponentReal]() {initLogics(graphicsComponentReal.get()); };

			componentCounter[graphicsComponentReal->getTypeName()]++;
		}

		// physics component
		else if constexpr (std::is_base_of<LP::CollisionBody, Component>::value)
		{
			auto physicsComponentReal = std::make_shared<Component>();
			physicsComponent = physicsComponentReal;
			initPhysicsComponent = [initLogics, physicsComponentReal](){initLogics(physicsComponentReal.get()); };
		}
		return *this;
	}

	virtual ~LActor() = default;

	static const auto& getComponentCounter() { return componentCounter;}

	std::shared_ptr<LG::LGraphicsComponent> graphicsComponent;
	std::shared_ptr<LP::RigidBody> physicsComponent;

protected:

	virtual void beginPlay();
	virtual void endPlay(){}

	std::function<void()> initGraphicsComponent;
	std::function<void()> initPhysicsComponent;

	static std::unordered_map<std::string, uint32> componentCounter;

	// DEBUG_CODE(std::shared_ptr<LG::LGraphicsComponent> debugRenderComponent;)
};

