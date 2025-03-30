#include "LActor.h"
#include "LEngine.h"

std::unordered_map<std::string, uint32> LActor::componentCounter;

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
		const_cast<std::function<glm::mat4x4()>&>(graphicsComponent->getModelMatrix) = [this]()
		{
			return physicsComponent->transform.getAsMatrix();
		};
	}
}
