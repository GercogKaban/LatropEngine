#include "LEngine.h"
#include <glfw3.h>
#include <dynamics/BasicSolver.h>

PlayerCharacter* PlayerCharacter::thisPtr = nullptr;
LEngine* LEngine::thisPtr = nullptr;

DEBUG_CODE(bool ObjectBuilder::bIsConstructing = false;)

PlayerCharacter::PlayerCharacter()
{
	thisPtr = this;
	renderer = LRenderer::get();

	glfwSetKeyCallback(renderer->getWindow(), handleInput);
	glfwSetCursorPosCallback(renderer->getWindow(), mouseInput);
}

void PlayerCharacter::tick(float delta)
{
	glm::vec3 inputs = glm::vec3(0.0f);

	glm::vec3 cameraFront = renderer->getCameraFront();
	glm::vec3 cameraUp = renderer->getCameraUp();

	if (isKeyPressed(GLFW_KEY_W))
	{
		inputs += cameraFront * getSpeed() * delta;
	}
	if (isKeyPressed(GLFW_KEY_S))
	{
		inputs -= cameraFront * getSpeed() * delta;
	}
	if (isKeyPressed(GLFW_KEY_A))
	{
		inputs -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * delta;
	}
	if (isKeyPressed(GLFW_KEY_D))
	{
		inputs += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * delta;
	}

	inputs.y = 0.0f;

	if (inputs != glm::vec3(0.0f))
	{
		location += inputs;
		updateCamera(location);
	}
}

bool PlayerCharacter::isKeyPressed(int32 keyCode) const
{
	if (auto key = pressedKeys.find(keyCode); key != pressedKeys.end())
	{
		return key->second == GLFW_PRESS;
	}
	return false;
}

void PlayerCharacter::handleInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (PlayerCharacter* playerCharacter = PlayerCharacter::get())
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			playerCharacter->pressedKeys[key] = GLFW_PRESS;
		}
		else if (action == GLFW_RELEASE)
		{
			playerCharacter->pressedKeys[key] = GLFW_RELEASE;
		}
	}
}

void PlayerCharacter::mouseInput(GLFWwindow* window, double xpos, double ypos)
{
	if (PlayerCharacter* playerCharacter = PlayerCharacter::get())
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

		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		LRenderer::get()->setCameraFront(glm::normalize(front));

		glfwSetCursorPos(window, playerCharacter->centerX, playerCharacter->centerY);
		playerCharacter->lastX = playerCharacter->centerX;
		playerCharacter->lastY = playerCharacter->centerY;
	}
}

void PlayerCharacter::updateCamera(const glm::vec3& newLocation)
{
	renderer->setCameraPosition(newLocation);
}

LEngine::LEngine(const LWindow& window)
	:LRenderer(window)
{
	thisPtr = this;
	physicsWorld.addSolver(std::make_unique<LatropPhysics::BasicSolver>());
}

void LEngine::addTickablePrimitive(std::weak_ptr<LTickable> ptr)
{
	tickables.push_back(ptr);
}

void LEngine::loop()
{
	while (window && !glfwWindowShouldClose(window))
	{
		updateDelta();
		fpsTimer += getDelta();
		if (fpsTimer >= 1.0f)
		{
			fpsTimer = 0.0f;
			fps = 0;
		}

		executeTickables();
		glfwPollEvents();

		if (bNeedToUpdateProjView)
		{
			updateProjView();
		}

		physicsWorld.integrate(getDelta());

		for (auto object : objects)
		{
			auto rigidObject = std::dynamic_pointer_cast<LatropPhysics::RigidBody>(object->physicsComponent);

			if (rigidObject)
			{
				//object->renderComponent->setModelMatrix(rigidObject->transform->getAsMatrix());
			}
		}

		drawFrame();
		fps++;
	}
	vkDeviceWaitIdle(logicalDevice);
}

void LEngine::executeTickables()
{
	for (auto it = tickables.begin(); it != tickables.end(); ++it)
	{
		if (!it->expired())
		{
			dynamic_cast<LTickable*>(it->lock().get())->tick(getDelta());
		}
		else
		{
			it = tickables.erase(it);
		}
	}
}
