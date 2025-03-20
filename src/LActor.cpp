#include "LActor.h"
#include "LEngine.h"

void LActor::beginPlay()
{
	if (initGraphicsComponent)
	{
		initGraphicsComponent();
		initGraphicsComponent = [](){};
		ObjectBuilder::registerComponent(graphicsComponent);
	}

	if (initPhysicsComponent)
	{
		initPhysicsComponent();
		initPhysicsComponent = [](){};
		ObjectBuilder::registerComponent(physicsComponent);
	}

	if (graphicsComponent)
	{
		graphicsComponent->getModelMatrix = [this]()
		{
			return physicsComponent->transform.getAsMatrix();
		};
	}
}
