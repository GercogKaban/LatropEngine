#pragma once

#include <Primitives.h>
#include <dynamics/RigidBody.h>

// represents a physical object with visualization
class LActor
{
public:

	friend class ObjectBuilder;

	LActor(std::shared_ptr<LG::LGraphicsComponent> graphicsComponent, std::shared_ptr<LatropPhysics::RigidBody> physicsComponent)
		: graphicsComponent(graphicsComponent), 
		  physicsComponent(physicsComponent)
	{
		if (graphicsComponent)
		{
			graphicsComponent->getModelMatrix = [physicsComponent]()
			{
				return physicsComponent->transform.getAsMatrix();
			};
		}
	}

	virtual ~LActor() = default;

	std::shared_ptr<LG::LGraphicsComponent> graphicsComponent;
	std::shared_ptr<LatropPhysics::RigidBody> physicsComponent;

protected:

	// DEBUG_CODE(std::shared_ptr<LG::LGraphicsComponent> debugRenderComponent;)
};

