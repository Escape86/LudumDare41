#include "MapTile.h"
#include "Display.h"
#include "Constants.h"
#include <map>
#include <algorithm>

#if _DEBUG
	#include <assert.h>
#endif

std::map<int, TileInfo> westernTileIdToInfoLookup		// { id, { spriteSheetRowOffset, spriteSheetColumnOffset, walkAble }}
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
	{ 32, { 10, 2, false } },
	{ 33, { 11, 0, true } },
	{ 34, { 11, 1, true } },
	{ 35, { 11, 2, true } },
	{ 36, { 12, 0, true } },
	{ 37, { 12, 1, true } },
	{ 38, { 12, 2, true } },
	{ 39, { 13, 0, false } },
	{ 40, { 13, 1, true } },
	{ 41, { 13, 2, false } },
};

std::map<int, TileInfo> bankTileIdToInfoLookup		// { id, { spriteSheetRowOffset, spriteSheetColumnOffset, walkAble }}
{
	{ 0,  { 0, 0, false } },
	{ 1,  { 0, 1, false } },
	{ 2,  { 0, 2, true } },
	{ 3,  { 0, 3, true } },
	{ 4,  { 0, 4, false } },
	{ 5,  { 0, 5, false } },
	{ 6,  { 1, 0, false } },
	{ 7,  { 1, 1, false } },
	{ 8,  { 1, 2, false } },
	{ 9,  { 1, 3, false } },
	{ 10, { 1, 4, false } },
	{ 11, { 1, 5, false } },
	{ 12, { 2, 0, false } },
	{ 13, { 2, 1, false } },
	{ 14, { 2, 2, false } },
	{ 15, { 2, 3, false } },
	{ 16, { 2, 4, false } },
	{ 17, { 2, 5, false } },
	{ 18, { 3, 0, false } },
	{ 19, { 3, 1, true } },
	{ 20, { 3, 2, true } },
	{ 21, { 3, 3, true } },
	{ 22, { 3, 4, false } },
	{ 23, { 3, 5, false } },
	{ 24, { 4, 0, true } },
	{ 25, { 4, 1, true } },
	{ 26, { 4, 2, true } },
	{ 27, { 4, 3, false } },
	{ 28, { 4, 4, false } },
	{ 29, { 4, 5, false } },
	{ 30, { 5, 0, false } },
	{ 31, { 5, 1, false } },
	{ 32, { 5, 2, false } },
	{ 33, { 5, 3, false } },
	{ 34, { 5, 4, false } },
	{ 35, { 5, 5, false } },
};

std::map<int, TileInfo> catHouseTileIdToInfoLookup		// { id, { spriteSheetRowOffset, spriteSheetColumnOffset, walkAble }}
{
	{ 0,{ 0, 0, false } },
	{ 1,{ 0, 1, false } },
	{ 2,{ 0, 2, true } },
	{ 3,{ 0, 3, true } },
	{ 4,{ 0, 4, false } },
	{ 5,{ 0, 5, false } },
	{ 6,{ 1, 0, false } },
	{ 7,{ 1, 1, false } },
	{ 8,{ 1, 2, false } },
	{ 9,{ 1, 3, false } },
	{ 10,{ 1, 4, false } },
	{ 11,{ 1, 5, false } },
	{ 12,{ 2, 0, false } },
	{ 13,{ 2, 1, false } },
	{ 14,{ 2, 2, false } },
	{ 15,{ 2, 3, false } },
	{ 16,{ 2, 4, false } },
	{ 17,{ 2, 5, false } },
	{ 18,{ 3, 0, false } },
	{ 19,{ 3, 1, true } },
	{ 20,{ 3, 2, true } },
	{ 21,{ 3, 3, true } },
	{ 22,{ 3, 4, false } },
	{ 23,{ 3, 5, false } },
	{ 24,{ 4, 0, true } },
	{ 25,{ 4, 1, true } },
	{ 26,{ 4, 2, true } },
	{ 27,{ 4, 3, false } },
	{ 28,{ 4, 4, false } },
	{ 29,{ 4, 5, false } },
	{ 30,{ 5, 0, false } },
	{ 31,{ 5, 1, false } },
	{ 32,{ 5, 2, false } },
	{ 33,{ 5, 3, false } },
	{ 34,{ 5, 4, false } },
	{ 35,{ 5, 5, false } },
};

std::map<int, TileInfo> houseInterriorTileIdToInfoLookup		// { id, { spriteSheetRowOffset, spriteSheetColumnOffset, walkAble }}
{
	{ 0,{ 0, 0, false } },
	{ 1,{ 0, 1, false } },
	{ 2,{ 0, 2, true } },
	{ 3,{ 0, 3, true } },
	{ 4,{ 0, 4, false } },
	{ 5,{ 0, 5, false } },
	{ 6,{ 1, 0, false } },
	{ 7,{ 1, 1, false } },
	{ 8,{ 1, 2, false } },
	{ 9,{ 1, 3, false } },
	{ 10,{ 1, 4, false } },
	{ 11,{ 1, 5, false } },
	{ 12,{ 2, 0, false } },
	{ 13,{ 2, 1, false } },
	{ 14,{ 2, 2, false } },
	{ 15,{ 2, 3, false } },
	{ 16,{ 2, 4, false } },
	{ 17,{ 2, 5, false } },
	{ 18,{ 3, 0, false } },
	{ 19,{ 3, 1, true } },
	{ 20,{ 3, 2, true } },
	{ 21,{ 3, 3, true } },
	{ 22,{ 3, 4, false } },
	{ 23,{ 3, 5, false } },
	{ 24,{ 4, 0, true } },
	{ 25,{ 4, 1, true } },
	{ 26,{ 4, 2, true } },
	{ 27,{ 4, 3, false } },
	{ 28,{ 4, 4, false } },
	{ 29,{ 4, 5, false } },
	{ 30,{ 5, 0, false } },
	{ 31,{ 5, 1, false } },
	{ 32,{ 5, 2, false } },
	{ 33,{ 5, 3, false } },
	{ 34,{ 5, 4, false } },
	{ 35,{ 5, 5, false } },
};

std::map<int, TileInfo> saloonTileIdToInfoLookup		// { id, { spriteSheetRowOffset, spriteSheetColumnOffset, walkAble }}
{
	{ 0,{ 0, 0, false } },
	{ 1,{ 0, 1, false } },
	{ 2,{ 0, 2, true } },
	{ 3,{ 0, 3, true } },
	{ 4,{ 0, 4, false } },
	{ 5,{ 0, 5, false } },
	{ 6,{ 1, 0, false } },
	{ 7,{ 1, 1, false } },
	{ 8,{ 1, 2, false } },
	{ 9,{ 1, 3, false } },
	{ 10,{ 1, 4, false } },
	{ 11,{ 1, 5, false } },
	{ 12,{ 2, 0, false } },
	{ 13,{ 2, 1, false } },
	{ 14,{ 2, 2, false } },
	{ 15,{ 2, 3, false } },
	{ 16,{ 2, 4, false } },
	{ 17,{ 2, 5, false } },
	{ 18,{ 3, 0, false } },
	{ 19,{ 3, 1, true } },
	{ 20,{ 3, 2, true } },
	{ 21,{ 3, 3, true } },
	{ 22,{ 3, 4, false } },
	{ 23,{ 3, 5, false } },
	{ 24,{ 4, 0, true } },
	{ 25,{ 4, 1, true } },
	{ 26,{ 4, 2, true } },
	{ 27,{ 4, 3, false } },
	{ 28,{ 4, 4, false } },
	{ 29,{ 4, 5, false } },
	{ 30,{ 5, 0, false } },
	{ 31,{ 5, 1, false } },
	{ 32,{ 5, 2, false } },
	{ 33,{ 5, 3, false } },
	{ 34,{ 5, 4, false } },
	{ 35,{ 5, 5, false } },
};

std::map<std::string, int> mapFileNameToMapUniqueIDLookup
{
	{ "resources/western.csv", 0 },
	{ "resources/bank.csv", 1 },
	{ "resources/cathouse.csv", 2 },
	{ "resources/saloon.csv", 3 },
	{ "resources/house_interrior.csv", 4 },
};

std::map<int, std::map<int, TileInfo>> mapFileNameToTileIdToInfoLookup
{
	{ 0, westernTileIdToInfoLookup },
	{ 1, bankTileIdToInfoLookup },
	{ 2, catHouseTileIdToInfoLookup },
	{ 3, saloonTileIdToInfoLookup },
	{ 4, houseInterriorTileIdToInfoLookup },
};

#pragma region Constructor

MapTile::MapTile(const std::string mapFileNameName, const int id, const int worldGridRow, const int worldGridColumn)
	: id(id), worldGridRow(worldGridRow), worldGridColumn(worldGridColumn)
{
	this->mapUniqueId = mapFileNameToMapUniqueIDLookup[mapFileNameName];

	this->walkable = mapFileNameToTileIdToInfoLookup[this->mapUniqueId][this->id].walkAble;
}

#pragma endregion

#pragma region Public Methods

MapTile::~MapTile()
{
	
}

void MapTile::Draw(Texture* texture, int cameraShiftX, int cameraShiftY)
{
	Display::QueueTextureForRendering(texture, (this->worldGridColumn * TILE_WIDTH) - (TILE_WIDTH / 2) - cameraShiftX, (this->worldGridRow * TILE_HEIGHT) - (TILE_HEIGHT / 2) - cameraShiftY, TILE_WIDTH, TILE_HEIGHT, false, true, mapFileNameToTileIdToInfoLookup[this->mapUniqueId][this->id].spriteSheetColumnOffset * TILE_WIDTH, mapFileNameToTileIdToInfoLookup[this->mapUniqueId][this->id].spriteSheetRowOffset * TILE_HEIGHT);
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
