#pragma once

#include "LActor.h"
#include "LTickable.h"

class LPlayerCharacter : public LActor, public LTickable
{

public:

	LPlayerCharacter()
	{

	}

	LPlayerCharacter(const glm::vec3& startPosition);

	virtual void tick(float delta) override;

	float getSpeed() const
	{
		return speed;
	}

	static LPlayerCharacter* get()
	{
		return thisPtr;
	}

protected:

	virtual void beginPlay() override;

	bool isKeyPressed(int32 keyCode) const;

	static void handleInput(class GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseInput(class GLFWwindow* window, double xpos, double ypos);

	void updateCamera(const glm::vec3& newLocation);
	void jump();

	std::unordered_map<int32, bool> pressedKeys;

	class LRenderer* renderer = nullptr;
	static LPlayerCharacter* thisPtr;
	float speed = 1.25f;

	// should be in camera class
	bool bFirstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 600.0f;
	float lastY = 300.0f;
	float sensitivity = 0.1f;

	float centerX = 1920 / 2;
	float centerY = 1080 / 2;
};
