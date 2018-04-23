#pragma once

#include "Object.h"
#include "Constants.h"

#pragma region Forward Declarations
struct SDL_Rect;
#pragma endregion

class Player : public Object
{
public:
	Player(double spawnX, double spawnY, Direction initialFacing);
	~Player();

	void InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime) override;
	void Draw() override;
	void OnKeyDown(int key);
	void OnKeyUp(int key);

	void ResetHorizontalVelocity();
	void ResetVerticalVelocity();

	int GetHp();
	void SetHp(int hp);

	const SDL_Rect* GetPlayerAttackHitBox();

private:
	void updateSpriteSheetOffsets();
	void attack();

	int horizontalVelocity;
	int verticalVelocity;

	int hp;

	bool keydownPrimed;
	bool animationFlag;

	int animationSwapCooldown;

	bool isAttacking;
	float attackDurationRemaining;
	float attackCooldownRemaining;
	SDL_Rect* attackHitBox;
};