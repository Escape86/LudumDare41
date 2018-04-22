#pragma once

#include "Object.h"

class Spawn : public Object
{
public:
	Spawn(double spawnX, double spawnY, int width, int height, std::string texturePath);
	~Spawn();

	void InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime) override;
	void Draw() override;
};