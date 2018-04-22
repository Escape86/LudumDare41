#pragma once

#include <string>

#pragma region Forward Declarations
class Texture;
#pragma endregion

struct TileInfo
{
	int textureOffsetX;
	int textureOffsetY;
};

class MapTile
{
public:
	MapTile(const int id, const int row, const int column);
	~MapTile();

	void Draw(Texture* texture, int cameraShiftX, int cameraShiftY);

private:
	const int id;
	const int row;
	const int column;
};