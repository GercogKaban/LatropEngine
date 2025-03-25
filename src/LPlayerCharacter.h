#pragma once

#include "LActor.h"
#include "LTickable.h"
#include <glm/gtc/quaternion.hpp>

class LPlayerCharacter : public LActor, public LTickable
{

public:

	LPlayerCharacter()
	{

	}

	LPlayerCharacter(const glm::vec3& startPosition);

	virtual void tick(float delta) override;
	void resetJump() { jumpsCounter = 0; }

	float getSpeed() const;

	static LPlayerCharacter* get()
	{
		return thisPtr;
	}

	static const glm::vec3 standingDimensions;
	static const glm::vec3 crouchedDimensions;
	static const float mass;

protected:

	virtual void beginPlay() override;

	bool isKeyPressed(int32 keyCode) const;

	static void handleInput(class GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseInput(class GLFWwindow* window, double xpos, double ypos);

	void updateCamera();
	void jump();
	void crouch();
	void uncrouch();

	std::unordered_map<int32, bool> pressedKeys;

	class LRenderer* renderer = nullptr;
	static LPlayerCharacter* thisPtr;

	// should be in camera class
	bool bFirstMouse = true;
	glm::quat orientation;
	float lastX = 600.0f;
	float lastY = 300.0f;
	float sensitivity = 0.1f;

	float centerX = 1920 / 2;
	float centerY = 1080 / 2;

private:
	static const int maxJumpsCount = 2;
	int jumpsCounter = maxJumpsCount;
};
