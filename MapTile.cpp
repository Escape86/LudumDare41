#include "MapTile.h"
#include "Display.h"
#include "Constants.h"
#include <map>

std::map<int, TileInfo> tileIdToInfoLookup		// { id, { columnOffset, rowOffset }}
{
	{ 0, { 0, 0 } },
	{ 1, { 1, 0 } },
	{ 2, { 0, 1 } },
	{ 3, { 1, 1 } },
};

#pragma region Constructor

MapTile::MapTile(const int id, const int row, const int column)
	: id(id), row(row), column(column)
{
}

#pragma endregion

#pragma region Public Methods

MapTile::~MapTile()
{
	
}

void MapTile::Draw(Texture* texture, int cameraShiftX, int cameraShiftY)
{
	Display::QueueTextureForRendering(texture, (this->column * TILE_WIDTH) - cameraShiftX, (this->row * TILE_HEIGHT) - cameraShiftY, TILE_WIDTH, TILE_HEIGHT, false, true, tileIdToInfoLookup[this->id].textureOffsetX * TILE_WIDTH, tileIdToInfoLookup[this->id].textureOffsetY * TILE_HEIGHT);
}

#pragma endregion
