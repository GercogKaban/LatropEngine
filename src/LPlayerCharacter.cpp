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
	float runningMultiplier = (isKeyPressed(GLFW_KEY_LEFT_SHIFT) ? 2.0 : 1.0) * 0.5;
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

		// Update camera
		playerCharacter->updateCamera();

		// Compute the new camera direction from the quaternion
		glm::vec3 front = playerCharacter->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
		LRenderer::get()->setCameraFront(glm::normalize(front));
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
	// float heightDifference = (standingDimensions.y - crouchedDimensions.y) / 2.0f;
	// physicsComponent->transform.position.y -= heightDifference;
	// physicsComponent->transform.scale = crouchedDimensions;
	physicsComponent->linearVelocity = glm::vec3(0.0f);
	physicsComponent->angularVelocity = glm::vec3(0.0f);
}

void LPlayerCharacter::uncrouch()
{
	// float heightDifference = (standingDimensions.y - crouchedDimensions.y) / 2.0f;
	// physicsComponent->transform.position.y += heightDifference;
	// physicsComponent->transform.scale = standingDimensions;
}