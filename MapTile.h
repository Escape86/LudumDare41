#pragma once

#include <string>

#pragma region Forward Declarations
class Texture;
#pragma endregion

struct TileInfo
{
	int spriteSheetRowOffset;
	int spriteSheetColumnOffset;
	bool walkAble;
};

class MapTile
{
public:
	MapTile(const int id, const int worldGridRow, const int worldGridColumn);
	~MapTile();

	void Draw(Texture* texture, int cameraShiftX, int cameraShiftY);

	int GetWorldGridRow() const;
	int GetWorldGridColumn() const;

	bool GetIsWalkable() const;

private:
	const int id;
	const int worldGridRow;
	const int worldGridColumn;
	bool walkable;
};