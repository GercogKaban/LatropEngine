#pragma once

#include <LRenderer.h>
#include <collision/CollisionBody.h>

// represents a physical object with visualization
class LActor
{
public:

	friend class ObjectBuilder;

	LActor() = default;
	~LActor() = default;

	std::shared_ptr<LatropPhysics::CollisionBody> physicsComponent;
	std::shared_ptr<LG::LGraphicsComponent> renderComponent;

protected:

	DEBUG_CODE(std::shared_ptr<LG::LGraphicsComponent> debugRenderComponent;)
};

