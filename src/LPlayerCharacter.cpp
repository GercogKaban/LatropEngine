#include "LPlayerCharacter.h"
#include <LRenderer.h>
#include <glfw3.h>

LPlayerCharacter* LPlayerCharacter::thisPtr = nullptr;

LPlayerCharacter::LPlayerCharacter(const glm::vec3& startPosition)
{
	thisPtr = this;
}

void LPlayerCharacter::tick(float delta)
{
	glm::vec3 inputs = glm::vec3(0.0f);

	glm::vec3 cameraFront = renderer->getCameraFront();
	glm::vec3 cameraUp = renderer->getCameraUp();

	// Project the front vector onto the horizontal plane (Y = 0)
	glm::vec3 horizontalFront = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
	glm::vec3 right = glm::normalize(glm::cross(horizontalFront, cameraUp));

	if (isKeyPressed(GLFW_KEY_W))
	{
		inputs += horizontalFront * getSpeed() * delta;
	}
	if (isKeyPressed(GLFW_KEY_S))
	{
		inputs -= horizontalFront * getSpeed() * delta;
	}
	if (isKeyPressed(GLFW_KEY_A))
	{
		inputs -= right * getSpeed() * delta;
	}
	if (isKeyPressed(GLFW_KEY_D))
	{
		inputs += right * getSpeed() * delta;
	}

	inputs.y = 0.0f;

	if (inputs != glm::vec3(0.0f))
	{
		physicsComponent->transform.position += inputs;
	}
	updateCamera(physicsComponent->transform.position);
}

void LPlayerCharacter::beginPlay()
{
	LActor::beginPlay();

	LPlayerCharacter::thisPtr = this;

	renderer = LRenderer::get();
	glfwSetKeyCallback(renderer->getWindow(), handleInput);
	glfwSetCursorPosCallback(renderer->getWindow(), mouseInput);
	updateCamera(physicsComponent->transform.position);
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

		playerCharacter->yaw += xoffset;
		playerCharacter->pitch -= yoffset;

		auto pitch = playerCharacter->pitch;
		auto yaw = playerCharacter->yaw;

		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}

		glm::vec3 front
		{
			cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
			sin(glm::radians(pitch)),
			cos(glm::radians(pitch)) * sin(glm::radians(yaw))
		};

		LRenderer::get()->setCameraFront(glm::normalize(front));

		glfwSetCursorPos(window, playerCharacter->centerX, playerCharacter->centerY);
		playerCharacter->lastX = playerCharacter->centerX;
		playerCharacter->lastY = playerCharacter->centerY;
	}
}

void LPlayerCharacter::updateCamera(const glm::vec3& newLocation)
{
	renderer->setCameraPosition(newLocation);
}

void LPlayerCharacter::jump()
{
	// Consider the player grounded if the vertical velocity is close to threshold
	if (std::abs(physicsComponent->m_velocity.y) < 0.65f)
	{
		physicsComponent->m_velocity.y += 5;
	}
}