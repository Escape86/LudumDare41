#pragma once

#include <string>

#pragma region Forward Declarations
class Texture;
struct SDL_Rect;
#pragma endregion

enum Direction
{
	NONE = -1,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Object
{
	friend class Teleporter;
	friend class Trigger;

public:
	Object(double spawnX, double spawnY, int width, int height, std::string texturePath);
	virtual ~Object();

	virtual void InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime) = 0;

	virtual void Draw();

	bool TestCollision(Object* otherObject);
	bool TestCollisionWithRect(const SDL_Rect* thing);
	void SetTexture(std::string texturePath);

	double GetPositionX() const;
	double GetPositionY() const;

	void SetPosition(double x, double y);

	Direction GetFacing();

protected:
	double x;
	double y;
	const int width;
	const int height;
	Direction facing;
	Texture* texture;
	int spriteSheetOffsetX;
	int spriteSheetOffsetY;
};