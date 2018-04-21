#include "Object.h"
#include "Texture.h"
#include "Display.h"

#pragma region Constructor

Object::Object(double x, double y, std::string texturePath)
{
	this->x = x;
	this->y = y;

	this->SetTexture(texturePath);
}

#pragma endregion

#pragma region Public Methods

Object::~Object()
{
	if (this->texture)
	{
		delete this->texture;
	}
}

void Object::Draw()
{
	Display::QueueTextureForRendering(this->texture, this->x, this->y);
}

bool Object::TestCollision(Object* otherObject)
{
	const int thisHalfWidth = this->width / 2;
	const int thisHalfHeight = this->height / 2;
	const int otherHalfWidth = otherObject->width / 2;
	const int otherHalfHeight = otherObject->height / 2;

	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = this->x - thisHalfWidth;
	rightA = this->x + thisHalfWidth;
	topA = this->y - thisHalfHeight;
	bottomA = this->y + thisHalfHeight;

	//Calculate the sides of rect B
	leftB = otherObject->x - otherHalfWidth;
	rightB = otherObject->x + otherHalfWidth;
	topB = otherObject->y - otherHalfHeight;
	bottomB = otherObject->y + otherHalfHeight;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

void Object::SetTexture(std::string texturePath)
{
	if (this->texture)
	{
		delete this->texture;
	}

	this->texture = new Texture(texturePath);
	this->texture->Load();

	//TODO: pulling this from texture size is probably not ideal
	this->width = this->texture->GetWidth();
	this->height = this->texture->GetHeight();
}

#pragma endregion
