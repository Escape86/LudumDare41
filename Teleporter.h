#pragma once

#include <string>

#pragma region Forward Declarations
class Object;
#pragma endregion

struct Destination
{
	std::string destinationMapFilePath;
	std::string destinationMapTextureFilePath;
	std::string destinationSpawnsFilePath;
	std::string destinationTeleportersFilePath;
	int destinationX;
	int destinationY;
};

class Teleporter
{
public:
	Teleporter(int x, int y, int width, int height, std::string destinationMapFilePath, std::string destinationMapTextureFilePath, std::string destinationSpawnsFilePath, std::string destinationTeleportersFilePath, int destinationX, int destinationY);
	~Teleporter();

	bool TestCollision(Object* otherObject) const;
	const Destination& GetDestination() const;

private:
	int x;
	int y;
	int width;
	int height;

	Destination destination;
};