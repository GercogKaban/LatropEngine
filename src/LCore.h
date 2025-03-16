#pragma once

#include <LRenderer.h>
#include <dynamics/RigidBody.h>

// represents a physical object with visualization
class LActor: public LG::LGraphicsComponent, public LatropPhysics::RigidBody
{
public:

	friend class ObjectBuilder;

	LActor(bool takesGrabity) : LG::LGraphicsComponent(), LatropPhysics::RigidBody() 
	{
		m_takesGravity = takesGrabity;
	}
	
	~LActor() = default;

	const glm::mat4x4 getModelMatrix() const override
	{
		return transform.getAsMatrix();
	}

protected:

	DEBUG_CODE(std::shared_ptr<LG::LGraphicsComponent> debugRenderComponent;)
};

