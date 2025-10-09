#include "LPlayerCharacter.h"
#include <LRenderer.h>
#include <glfw3.h>

LPlayerCharacter* LPlayerCharacter::thisPtr = nullptr;
const glm::vec3 LPlayerCharacter::standingDimensions = glm::vec3(0.5, 1.6f, /*0.3*/0.5f);
const glm::vec3 LPlayerCharacter::crouchedDimensions = glm::vec3(0.5, 0.8f, /*0.3*/0.5f);
const float LPlayerCharacter::mass = 65.0f;

LPlayerCharacter::LPlayerCharacter(const glm::vec3& startPosition)
{
	thisPtr = this;
}

float LPlayerCharacter::getSpeed() const
{
	static float walkingSpeed = 1.45f;
	static float crouchedSpeed = 0.87f;
	float runningMultiplier = (isKeyPressed(GLFW_KEY_LEFT_SHIFT) ? 2.0 : 1.0);
	float speed = isKeyPressed(GLFW_KEY_LEFT_CONTROL) ? crouchedSpeed : walkingSpeed;

	return speed * runningMultiplier;
}

void LPlayerCharacter::tick(float delta)
{
	glm::vec3 velocity = glm::vec3(0.0f);

	glm::vec3 cameraFront = renderer->getCameraFront();
	glm::vec3 cameraUp = renderer->getCameraUp();

	// Project the front vector onto the horizontal plane (Y = 0)
	glm::vec3 horizontalFront = glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
	glm::vec3 right = glm::cross(horizontalFront, cameraUp);
	right.y = 0.0f;

	if (isKeyPressed(GLFW_KEY_W)) velocity += horizontalFront;
	if (isKeyPressed(GLFW_KEY_S)) velocity -= horizontalFront;
	if (isKeyPressed(GLFW_KEY_A)) velocity -= right;
	if (isKeyPressed(GLFW_KEY_D)) velocity += right;

	if (glm::length(velocity) > 0.0f)
	{
		velocity = glm::normalize(velocity) * getSpeed();
		physicsComponent->linearVelocity.x = velocity.x;
		physicsComponent->linearVelocity.z = velocity.z;
	}
	updateCamera();
}

void LPlayerCharacter::setOrientation(const glm::quat& newValue)
{
	this->orientation = newValue;
	// Update camera
	this->updateCamera();

	// Compute the new camera direction from the quaternion
	glm::vec3 front = this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
	LRenderer::get()->setCameraFront(glm::normalize(front));
}

void LPlayerCharacter::teleportThroughPortal(const LP::RigidBody* srcPortal, const LP::RigidBody* dstPortal)
{
    const auto& src = srcPortal->transform;
    const auto& dst = dstPortal->transform;

    // 180° flip so player exits facing outward
    glm::quat flip = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0));

    // delta rotation to convert src-space → dst-space (with facing flip)
    glm::quat delta = dst.rotation * glm::inverse(src.rotation);

    // translate position: preserve offset from source, then apply delta
    glm::vec3 worldPos = physicsComponent->transform.position;
    glm::vec3 newPos = dst.position + delta * (worldPos - src.position);

    // rotate orientation and velocity by delta
    glm::quat newOrient = glm::normalize(delta * this->orientation);
    glm::vec3 newVel = delta * physicsComponent->linearVelocity;

    // small safety nudge along destination normal
    glm::vec3 dstNormal = glm::normalize(dst.rotation * glm::vec3(0.0f, 1.0f, 0.0f));
    const float safetyOffset = 0.03f; // 0.12f;
    newPos += dstNormal * safetyOffset;

    // apply
    physicsComponent->transform.position = newPos;
    physicsComponent->linearVelocity = newVel;
    // setOrientation(newOrient);
	setOrientation(orientation * glm::angleAxis(glm::radians(180.0f) ,glm::vec3(0, 1, 0)));
}

void LPlayerCharacter::teleportTo(const LP::RigidBody* destinationPortal)
{
	// Get destination portal's transform
	auto destinationTransform = destinationPortal->transform;
	// destinationTransform.position.y -= 1.0f;

	// Compute teleport position: Move to portal position & offset slightly along normal
	glm::vec3 portalNormal = glm::normalize(destinationTransform.rotation * glm::vec3 { 0.0f, 1.0f, 0.0f });
	// Teleport position
	physicsComponent->transform.position = destinationTransform.position;// + portalNormal/* * 1.1f */; // Offset slightly to prevent instant re-trigger
	physicsComponent->linearVelocity = glm::reflect(physicsComponent->linearVelocity, portalNormal);

	// Rotate player 180 degrees around Y-axis
	// Create a reflection matrix
    // glm::mat3 R = glm::mat3(1.0f) - 2.0f * glm::outerProduct(portalNormal, portalNormal);

    // // Convert the orientation to a matrix
    // glm::mat3 rot = glm::mat3_cast(orientation);

    // // Reflect the rotation
    // glm::mat3 reflectedRot = R * rot * R;

	// setOrientation(glm::quat_cast(reflectedRot));
	setOrientation(orientation * glm::angleAxis(glm::radians(180.0f) ,glm::vec3(0, 1, 0)));
}

void LPlayerCharacter::beginPlay()
{
	LActor::beginPlay();

	LPlayerCharacter::thisPtr = this;

	renderer = LRenderer::get();
	glfwSetKeyCallback(renderer->getWindow(), handleInput);
	glfwSetCursorPosCallback(renderer->getWindow(), mouseInput);
	updateCamera();
}

bool LPlayerCharacter::isKeyPressed(int32 keyCode) const
{
	if (auto key = pressedKeys.find(keyCode); key != pressedKeys.end())
	{
		return key->second == GLFW_PRESS;
	}
	return false;
}

void LPlayerCharacter::handleInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (LPlayerCharacter* playerCharacter = LPlayerCharacter::get())
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			playerCharacter->pressedKeys[key] = GLFW_PRESS;
			if (key == GLFW_KEY_SPACE)
			{
				playerCharacter->jump();
			}
		}
		else if (action == GLFW_RELEASE)
		{
			playerCharacter->pressedKeys[key] = GLFW_RELEASE;

			if (key == GLFW_KEY_LEFT_CONTROL)
			{
				playerCharacter->uncrouch();
			}
		}

		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_LEFT_CONTROL)
			{
				playerCharacter->crouch();
			}
		}
	}
}

void LPlayerCharacter::mouseInput(GLFWwindow* window, double xpos, double ypos)
{
	if (LPlayerCharacter* playerCharacter = LPlayerCharacter::get())
	{
		float xoffset = xpos - playerCharacter->lastX;
		float yoffset = ypos - playerCharacter->lastY;

		xoffset *= playerCharacter->sensitivity;
		yoffset *= playerCharacter->sensitivity;

		// Create yaw rotation (global Y-axis)
		glm::quat yawQuat = glm::angleAxis(glm::radians(-xoffset), glm::vec3(0.0f, 1.0f, 0.0f));

		// Extract the current pitch angle from the quaternion
		glm::vec3 forward = playerCharacter->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
		float currentPitch = glm::degrees(asin(forward.y));  // Extract pitch from direction vector

		// Apply pitch change and clamp it to (-89, 89) degress
		float newPitch = glm::clamp(currentPitch - yoffset, -89.0f, 89.0f);
		float pitchDelta = glm::radians(newPitch - currentPitch);
		glm::quat pitchQuat = glm::angleAxis(pitchDelta, glm::vec3(1.0f, 0.0f, 0.0f));

		// Apply rotations (yaw globally, pitch locally)
		playerCharacter->orientation = yawQuat * playerCharacter->orientation;  // Apply yaw
		playerCharacter->orientation = playerCharacter->orientation * pitchQuat; // Apply pitch
		playerCharacter->orientation = glm::normalize(playerCharacter->orientation);

		// Reset cursor position
		glfwSetCursorPos(window, playerCharacter->centerX, playerCharacter->centerY);
		playerCharacter->lastX = playerCharacter->centerX;
		playerCharacter->lastY = playerCharacter->centerY;

		playerCharacter->setOrientation(playerCharacter->orientation);
	}
}

void LPlayerCharacter::updateCamera()
{
	glm::vec3 headPosition = physicsComponent->transform.position;
	// Head position is at 90-95% of the total height.
	// Here we get the scale divided by 2 (0.9 / 2 = 0.45) because the position
	// of the physicsCompontent is at the center of mass.
	headPosition.y += physicsComponent->transform.scale.y * 0.45f;
	renderer->setCameraPosition(headPosition);
	renderer->setCameraPositionToPlayer({ 0.0f, physicsComponent->transform.scale.y * 0.45f, 0.0f });
}

void LPlayerCharacter::jump()
{
	if (jumpsCounter < maxJumpsCount)
	{
		jumpsCounter += 1;
		if (physicsComponent->linearVelocity.y < 0)
		{
			physicsComponent->linearVelocity.y = 5;
		}
		else 
		{
			physicsComponent->linearVelocity.y += 5;
		}
	}
}

void LPlayerCharacter::crouch()
{
	float heightDifference = (standingDimensions.y - crouchedDimensions.y) / 2.0f;
	physicsComponent->transform.position.y -= heightDifference;
	physicsComponent->transform.scale = crouchedDimensions;
}

void LPlayerCharacter::uncrouch()
{
	float heightDifference = (standingDimensions.y - crouchedDimensions.y) / 2.0f;
	physicsComponent->transform.position.y += heightDifference;
	physicsComponent->transform.scale = standingDimensions;
}