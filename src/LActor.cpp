#include "LActor.h"
#include "LEngine.h"

std::unordered_map<LG::EPrimitiveType, uint32> LActor::componentCounter;

void LActor::beginPlay()
{
	if (initPhysicsComponent)
	{
		initPhysicsComponent();
		initPhysicsComponent = [](){};
		ObjectBuilder::registerComponent(physicsComponent);
	}

	if (initGraphicsComponent)
	{
		initGraphicsComponent();
		initGraphicsComponent = [](){};

		const_cast<std::function<glm::mat4x4()>&>(graphicsComponent->getModelMatrix) = [this]()
		{
			return physicsComponent->transform.getAsMatrix();
		};

		const_cast<std::function<bool()>&>(graphicsComponent->isAlwaysStatic) = [this]()
		{
			return !physicsComponent->isSimulated();
		};

		ObjectBuilder::registerComponent(graphicsComponent);
	}
}
