#include "Spawn.h"
#include "Constants.h"
#include "Game.h"
#include "Map.h"
#include "Display.h"

#if _DEBUG
	#include <assert.h>
#endif

#pragma region Constructor

Spawn::Spawn(int id, double spawnX, double spawnY, int width, int height, std::string texturePath, int spriteSheetOffsetX, int spriteSheetOffsetY) : Object(spawnX, spawnY, width, height, texturePath), id(id)
{
	this->spriteSheetOffsetX = spriteSheetOffsetX;
	this->spriteSheetOffsetY = spriteSheetOffsetY;
}

#pragma endregion

#pragma region Public Methods

Spawn::~Spawn()
{
}

void Spawn::InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime)
{
	//enforce screen bounds
	int halfWidth = this->width / 2;
	int halfHeight = this->height / 2;

	const Game* game = Game::GetInstance();
	const int mapWidth = game->GetMap()->GetColumnCount() * TILE_WIDTH;
	const int mapHeight = game->GetMap()->GetRowCount() * TILE_HEIGHT;

	if (this->x - halfWidth < 0)
	{
		this->x = halfWidth;
	}
	else if (this->x + halfWidth > mapWidth)
	{
		this->x = mapWidth - halfWidth;
	}

	if (this->y - halfHeight < 0)
	{
		this->y = halfHeight;
	}
	else if (this->y + halfHeight > mapHeight)
	{
		this->y = mapHeight - halfHeight;
	}
}

void Spawn::Draw()
{ 
	const Game* game = Game::GetInstance();
	const SDL_Rect& camera = game->GetCamera();

	Display::QueueTextureForRendering(this->texture, this->x - camera.x, this->y - camera.y, this->width, this->height, true, true, this->spriteSheetOffsetX, this->spriteSheetOffsetY);
}

#pragma endregion
