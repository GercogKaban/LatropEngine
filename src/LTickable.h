#pragma once

class LTickable
{
public:

	LTickable() = default;
	virtual ~LTickable() = default;

	virtual void tick(float delta) = 0;
};
