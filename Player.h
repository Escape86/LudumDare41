#pragma once

#include "Object.h"
#include "Constants.h"

class Player : public Object
{
public:
	Player();
	~Player();

	void InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime) override;
	void Draw() override;
	void OnKeyDown(int key);
	void OnKeyUp(int key);

	void ResetHorizontalVelocity();
	void ResetVerticalVelocity();

	int GetHp();
	void SetHp(int hp);

private:
	void onDirectionChange();

	int horizontalVelocity;
	int verticalVelocity;

	int hp;

	bool keydownPrimed;
};