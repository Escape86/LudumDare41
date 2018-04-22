#include "MapTile.h"
#include "Display.h"
#include "Constants.h"
#include <map>
#include <algorithm>

#if _DEBUG
	#include <assert.h>
#endif

std::map<int, TileInfo> tileIdToInfoLookup		// { id, { spriteSheetRowOffset, spriteSheetColumnOffset, walkAble }}
{
	{ 0,  { 0,  0, false } },
	{ 1,  { 0,  1, true } },
	{ 2,  { 0,  2, true } },
	{ 3,  { 1,  0, true } },
	{ 4,  { 1,  1, false } },
	{ 5,  { 1,  2, false } },
	{ 6,  { 2,  0, false } },
	{ 7,  { 2,  1, true } },
	{ 8,  { 2,  2, true } },
	{ 9,  { 3,  0, false } },
	{ 10, { 3,  1, false } },
	{ 11, { 3,  2, false } },
	{ 12, { 4,  0, false } },
	{ 13, { 4,  1, false } },
	{ 14, { 4,  2, false } },
	{ 15, { 5,  0, false } },
	{ 16, { 5,  1, false } },
	{ 17, { 5,  2, false } },
	{ 18, { 6,  0, false } },
	{ 19, { 6,  1, false } },
	{ 20, { 6,  2, false } },
	{ 21, { 7,  0, true } },
	{ 22, { 7,  1, false } },
	{ 23, { 7,  2, false } },
	{ 24, { 8,  0, false } },
	{ 25, { 8,  1, false } },
	{ 26, { 8,  2, false } },
	{ 27, { 9,  0, false } },
	{ 28, { 9,  1, false } },
	{ 29, { 9,  2, false } },
	{ 30, { 10, 0, false } },
	{ 31, { 10, 1, false } },
};

#pragma region Constructor

MapTile::MapTile(const int id, const int worldGridRow, const int worldGridColumn)
	: id(id), worldGridRow(worldGridRow), worldGridColumn(worldGridColumn)
{
	this->walkable = tileIdToInfoLookup[this->id].walkAble;
}

#pragma endregion

#pragma region Public Methods

MapTile::~MapTile()
{
	
}

void MapTile::Draw(Texture* texture, int cameraShiftX, int cameraShiftY)
{
	Display::QueueTextureForRendering(texture, (this->worldGridColumn * TILE_WIDTH) - cameraShiftX, (this->worldGridRow * TILE_HEIGHT) - cameraShiftY, TILE_WIDTH, TILE_HEIGHT, false, true, tileIdToInfoLookup[this->id].spriteSheetColumnOffset * TILE_WIDTH, tileIdToInfoLookup[this->id].spriteSheetRowOffset * TILE_HEIGHT);
}

int MapTile::GetWorldGridRow() const
{
	return this->worldGridRow;
}

int MapTile::GetWorldGridColumn() const
{
	return this->worldGridColumn;
}

bool MapTile::GetIsWalkable() const
{
	return this->walkable;
}

#pragma endregion
