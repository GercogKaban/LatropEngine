#pragma once

#include <LRenderer.h>
#include <dynamics/RigidBody.h>

// represents a physical object with visualization
class LActor
{
public:

	friend class ObjectBuilder;

	LActor(
		LG::LGFullGraphicsComponent *renderComponent,
		LatropPhysics::RigidBody *physicsComponent
	) : renderComponent(renderComponent), physicsComponent(physicsComponent) 
	{
		renderComponent->getModelMatrix = [physicsComponent](){ 
			return physicsComponent->transform.getAsMatrix(); 
		};
	}

	~LActor() = default;

	std::shared_ptr<LG::LGFullGraphicsComponent> renderComponent;
	std::shared_ptr<LatropPhysics::RigidBody> physicsComponent;

protected:

	DEBUG_CODE(std::shared_ptr<LG::LGFullGraphicsComponent> debugRenderComponent;)
};

