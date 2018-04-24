#pragma once

#include "Object.h"

class Spawn : public Object
{
public:
	Spawn(int id, double spawnX, double spawnY, int width, int height, std::string texturePath, int spriteSheetOffsetX, int spriteSheetOffsetY, bool shouldIdleMove);
	~Spawn();

	void InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime) override;
	void Draw() override;

	int GetID();

private:
	const int id;

	bool shouldIdleMove;
	int idleMoveCooldown;
	Direction idleDirection;
};