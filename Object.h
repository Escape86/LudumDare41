#pragma once

#include <string>

#pragma region Forward Declarations
class Texture;
#pragma endregion

class Object
{
public:
	Object(double x, double y, std::string texturePath);
	virtual ~Object();

	virtual void InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime) = 0;

	virtual void Draw();

	bool TestCollision(Object* otherObject);
	void SetTexture(std::string texturePath);

protected:
	double x;
	double y;
	int width;
	int height;

private:
	Texture* texture;
};