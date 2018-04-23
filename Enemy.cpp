#include "Enemy.h"
#include "Game.h"
#include "Player.h"

#if _DEBUG
	#include <assert.h>
#endif

#pragma region Constructor

Enemy::Enemy(int id, double spawnX, double spawnY, int width, int height, std::string texturePath, int spriteSheetOffsetX, int spriteSheetOffsetY, bool shouldIdleMove)
	: Spawn(id, spawnX, spawnY, width, height, texturePath, spriteSheetOffsetX, spriteSheetOffsetY, shouldIdleMove)
{

}

#pragma endregion

#pragma region Public Methods

Enemy::~Enemy()
{

}

void Enemy::InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime)
{
	float previousFrameTimeInSeconds = (previousFrameTime / 1000.0f);

	const Player* player = Game::GetInstance()->GetPlayer();
	
#if _DEBUG
	assert(player);
#endif

	int targetX = player->GetPositionX();
	int targetY = player->GetPositionY();

	float movementVelocity = ENEMY_VELOCITY * previousFrameTimeInSeconds;

	if (this->x > targetX)
	{
		this->x -= movementVelocity;
	}
	else if (this->x < targetX)
	{
		this->x += movementVelocity;
	}

	if (this->y > targetY)
	{
		this->y -= movementVelocity;
	}
	else if (this->y < targetY)
	{
		this->y += movementVelocity;
	}
}

#pragma endregion