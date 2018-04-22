#pragma once

#include "Object.h"
#include "Constants.h"

class Player : public Object
{
public:
	Player();
	~Player();

	void InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime) override;
	void Draw();
	void OnKeyDown(int key);
	void OnKeyUp(int key);

	enum Direction
	{
		FRONT_X = 0,
		FRONT_Y = 0,
		LEFT_X = PLAYER_WIDTH,
		LEFT_Y = PLAYER_HEIGHT,
		RIGHT_X = 0,
		RIGHT_Y = PLAYER_HEIGHT,
		BACK_X = PLAYER_WIDTH,
		BACK_Y = 0,

		//remember to add corresponding load functionality when adding new font
	};

	void ResetHorizontalVelocity();
	void ResetVerticalVelocity();

	int GetHp();
	void SetHp(int hp);

private:
	int horizontalVelocity;
	int verticalVelocity;
	int spriteSheetOffsetX; // For making the character change direction
	int spriteSheetOffsetY;

	int hp;

	bool keydownPrimed;
};