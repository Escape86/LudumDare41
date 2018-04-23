#include "Player.h"
#include "Texture.h"
#include "Display.h"
#include "Audio.h"
#include "Constants.h"
#include "Game.h"
#include "Map.h"
#include "MapTile.h"

#if _DEBUG
	#include <assert.h>
#endif

#pragma region Constructor

Player::Player() : Object(PLAYER_WIDTH + 150, PLAYER_HEIGHT + 150, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_TEXTURE_PATH)
{
	this->horizontalVelocity = 0;
	this->verticalVelocity = 0;

	this->spriteSheetOffsetX = 0;
	this->spriteSheetOffsetY = 0;

	this->hp = PLAYER_MAX_HP;

	//prevent level switching from causing keyups to occur without a corresponding keydown
	this->keydownPrimed = false;

	// animation switch bool
	this->animationFlag = false;
	this->animationSwapCooldown = 0;

	this->isAttacking = false;
	this->attackDurationRemaining = 0.0f;
	this->attackCooldownRemaining = 0.0f;
	this->attackHitBox = nullptr;
}

#pragma endregion

#pragma region Public Methods

Player::~Player()
{
}

void Player::InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime)
{
	float previousFrameTimeInSeconds = (previousFrameTime / 1000.0f);

	double startPosX = this->x;
	double startPosY = this->y;
	int startTileRow = static_cast<int>((this->y + (this->height / 2)) / TILE_HEIGHT);
	int startTileColumn = static_cast<int>((this->x + (this->width / 2)) / TILE_WIDTH);

	//update position
	this->x += (this->horizontalVelocity * previousFrameTimeInSeconds);
	this->y += (this->verticalVelocity * previousFrameTimeInSeconds);

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

	//check if we're attempting to cross to a new tile that isn't walkable
	int endTileRow = static_cast<int>((this->y + (this->height / 2)) / TILE_HEIGHT);
	int endTileColumn = static_cast<int>((this->x + (this->width / 2)) / TILE_WIDTH);

	if (startTileRow != endTileRow || startTileColumn != endTileColumn)
	{
		//we crossed into a new tile, check if it's walkable
		const MapTile* tile = Game::GetInstance()->GetMap()->GetTileByWorldGridLocation(endTileRow, endTileColumn);
		if (tile == nullptr || !tile->GetIsWalkable())
		{
			//not walkable, so move them back!
			this->x = startPosX;
			this->y = startPosY;
		}
	}

	if (this->animationSwapCooldown <= 0)
	{
		animationFlag = !animationFlag;
		this->animationSwapCooldown = PLAYER_ANIMATION_COOLDOWN;
	}
	else
	{
		this->animationSwapCooldown -= previousFrameTime;
	}

	if (this->isAttacking)
	{
		if (this->attackDurationRemaining > 0.0f)
		{
			this->attackDurationRemaining -= previousFrameTimeInSeconds;
		}
		else
		{
			this->isAttacking = false;
			this->attackHitBox = nullptr;
			this->attackCooldownRemaining = PLAYER_ATTACK_COOLDOWN;
		}
	}
	else
	{
		if (this->attackCooldownRemaining > 0.0f)
		{
			this->attackCooldownRemaining -= previousFrameTimeInSeconds;
		}
	}
}

void Player::Draw()
{ 
	this->updateSpriteSheetOffsets();

	const Game* game = Game::GetInstance();
	const SDL_Rect& camera = game->GetCamera();

	Display::QueueTextureForRendering(this->texture, this->x - camera.x, this->y - camera.y, this->width, this->height, true, true, this->spriteSheetOffsetX, this->spriteSheetOffsetY);

	if (this->isAttacking)
	{
		switch (this->facing)
		{
		case Direction::UP:
			Display::QueueRectangleForRendering(this->x - camera.x + (this->width / 2) - 8, this->y - camera.y - PLAYER_ATTACK_LENGTH + 1, PLAYER_ATTACK_WIDTH, PLAYER_ATTACK_LENGTH, 0xFF, 0x00, 0x00);
			break;
		case Direction::DOWN:
			Display::QueueRectangleForRendering(this->x - camera.x - 6, this->y + (this->height / 2) - camera.y - 5, PLAYER_ATTACK_WIDTH, PLAYER_ATTACK_LENGTH, 0xFF, 0x00, 0x00);
			break;
		case Direction::LEFT:
			Display::QueueRectangleForRendering(this->x - (this->width / 2) - camera.x - PLAYER_ATTACK_LENGTH + 7, this->y - camera.y + 4, PLAYER_ATTACK_LENGTH, PLAYER_ATTACK_WIDTH, 0xFF, 0x00, 0x00);
			break;
		case Direction::RIGHT:
			Display::QueueRectangleForRendering(this->x + (this->width / 2) - camera.x - 6, this->y - camera.y + 4, PLAYER_ATTACK_LENGTH, PLAYER_ATTACK_WIDTH, 0xFF, 0x00, 0x00);
			break;
		default:
#if _DEBUG
	assert(false);	//wtf direction?
#endif
			break;
		}
	}
}

void Player::OnKeyDown(int key)
{
	switch (key)
	{
		case SDLK_w:
		case SDLK_UP:
			this->verticalVelocity -= PLAYER_VELOCITY;
			this->facing = Direction::UP;
			break;
		case SDLK_s:
		case SDLK_DOWN:
			this->verticalVelocity += PLAYER_VELOCITY;
			this->facing = Direction::DOWN;
			break;
		case SDLK_a:
		case SDLK_LEFT:
			this->horizontalVelocity -= PLAYER_VELOCITY;
			this->facing = Direction::LEFT;
			break;
		case SDLK_d:
		case SDLK_RIGHT:
			this->horizontalVelocity += PLAYER_VELOCITY;
			this->facing = Direction::RIGHT;
			break;
		case 32:
			//attack
			this->attack();
			break;
	}

	//enforce velocity min/max values
	if (this->verticalVelocity > PLAYER_VELOCITY)
		this->verticalVelocity = PLAYER_VELOCITY;
	else if (this->verticalVelocity < -PLAYER_VELOCITY)
		this->verticalVelocity = -PLAYER_VELOCITY;

	if (this->horizontalVelocity > PLAYER_VELOCITY)
		this->horizontalVelocity = PLAYER_VELOCITY;
	else if (this->horizontalVelocity < -PLAYER_VELOCITY)
		this->horizontalVelocity = -PLAYER_VELOCITY;

	this->keydownPrimed = true;
}

void Player::OnKeyUp(int key)
{
	if (!this->keydownPrimed)
		return;

	switch (key)
	{
		case SDLK_w:
		case SDLK_UP:
			this->verticalVelocity += PLAYER_VELOCITY;
			break;
		case SDLK_s:
		case SDLK_DOWN:
			this->verticalVelocity -= PLAYER_VELOCITY;
			break;
		case SDLK_a:
		case SDLK_LEFT:
			this->horizontalVelocity += PLAYER_VELOCITY;
			break;
		case SDLK_d:
		case SDLK_RIGHT:
			this->horizontalVelocity -= PLAYER_VELOCITY;
			break;
	}

	//enforce velocity min/max values
	if (this->verticalVelocity > PLAYER_VELOCITY)
		this->verticalVelocity = PLAYER_VELOCITY;
	else if (this->verticalVelocity < -PLAYER_VELOCITY)
		this->verticalVelocity = -PLAYER_VELOCITY;

	if (this->horizontalVelocity > PLAYER_VELOCITY)
		this->horizontalVelocity = PLAYER_VELOCITY;
	else if (this->horizontalVelocity < -PLAYER_VELOCITY)
		this->horizontalVelocity = -PLAYER_VELOCITY;
}

void Player::ResetHorizontalVelocity()
{
	this->horizontalVelocity = 0;
}

void Player::ResetVerticalVelocity()
{
	this->verticalVelocity = 0;
}

int Player::GetHp()
{
	return this->hp;
}

void Player::SetHp(int hp)
{
	this->hp = hp;
}

const SDL_Rect* Player::GetPlayerAttackHitBox()
{
	return this->attackHitBox;
}

#pragma endregion

#pragma region Private Methods

void Player::updateSpriteSheetOffsets()
{
	if (this->verticalVelocity || this->horizontalVelocity)
	{
		//is moving so show the appropriate section of the sprite sheet
		switch (this->facing)
		{
		case Direction::UP:
			if (animationFlag)
			{
				this->spriteSheetOffsetX = 0;
				this->spriteSheetOffsetY = PLAYER_HEIGHT;
			}
			else
			{
				this->spriteSheetOffsetX = PLAYER_WIDTH;
				this->spriteSheetOffsetY = PLAYER_HEIGHT;
			}
			break;
		case Direction::DOWN:
			if (animationFlag)
			{
				this->spriteSheetOffsetX = 0;
				this->spriteSheetOffsetY = 0;
			}
			else
			{
				this->spriteSheetOffsetX = PLAYER_WIDTH;
				this->spriteSheetOffsetY = 0;
			}
			break;
		case Direction::LEFT:
			if (animationFlag)
			{
				this->spriteSheetOffsetX = PLAYER_WIDTH * 2;
				this->spriteSheetOffsetY = 0;
			}
			else
			{
				this->spriteSheetOffsetX = PLAYER_WIDTH * 2;
				this->spriteSheetOffsetY = PLAYER_HEIGHT;
			}
			break;
		case Direction::RIGHT:
			if (animationFlag)
			{
				this->spriteSheetOffsetX = PLAYER_WIDTH;
				this->spriteSheetOffsetY = PLAYER_HEIGHT * 2;
			}
			else
			{
				this->spriteSheetOffsetX = 0;
				this->spriteSheetOffsetY = PLAYER_HEIGHT * 2;
			}
			break;

		default:
#if _DEBUG
	assert(false);	//wtf direction is this?
#endif
			break;
		}
	}
	else
	{
		//is not moving, so show the idle section of the sprite sheet
		switch (this->facing)
		{
		case Direction::UP:
				this->spriteSheetOffsetX = 0;
				this->spriteSheetOffsetY = PLAYER_HEIGHT * 3;
			break;
		case Direction::DOWN:
				this->spriteSheetOffsetX = PLAYER_WIDTH * 2;
				this->spriteSheetOffsetY = PLAYER_HEIGHT * 2;
			break;
		case Direction::LEFT:
				this->spriteSheetOffsetX = PLAYER_WIDTH * 2;
				this->spriteSheetOffsetY = PLAYER_HEIGHT * 3;
			break;
		case Direction::RIGHT:
				this->spriteSheetOffsetX = PLAYER_WIDTH;
				this->spriteSheetOffsetY = PLAYER_HEIGHT * 3;
			break;

		default:
#if _DEBUG
	assert(false);	//wtf direction is this?
#endif
			break;
		}
	}
}

void Player::attack()
{
	if (this->attackCooldownRemaining > 0.0f)
		return;

	this->isAttacking = true;
	this->attackDurationRemaining = PLAYER_ATTACK_DURATION;

	this->attackHitBox = new SDL_Rect();

	switch (this->facing)
	{
	case Direction::UP:
		this->attackHitBox->x = this->x + (this->width / 2) - 8;
		this->attackHitBox->y = this->y - PLAYER_ATTACK_LENGTH + 1;
		this->attackHitBox->w = PLAYER_ATTACK_WIDTH;
		this->attackHitBox->h = PLAYER_ATTACK_LENGTH;
		break;
	case Direction::DOWN:
		this->attackHitBox->x = this->x - 6;
		this->attackHitBox->y = this->y + (this->height / 2) - 5;
		this->attackHitBox->w = PLAYER_ATTACK_WIDTH;
		this->attackHitBox->h = PLAYER_ATTACK_LENGTH;
		break;
	case Direction::LEFT:
		this->attackHitBox->x = this->x - (this->width / 2) - PLAYER_ATTACK_LENGTH + 7;
		this->attackHitBox->y = this->y + 4;
		this->attackHitBox->w = PLAYER_ATTACK_LENGTH;
		this->attackHitBox->h = PLAYER_ATTACK_WIDTH;
		break;
	case Direction::RIGHT:
		this->attackHitBox->x = this->x + (this->width / 2) - 6;
		this->attackHitBox->y = this->y + 4;
		this->attackHitBox->w = PLAYER_ATTACK_LENGTH;
		this->attackHitBox->h = PLAYER_ATTACK_WIDTH;
		break;
	default:
#if _DEBUG
		assert(false);	//wtf direction?
#endif
		break;
	}
}

#pragma endregion
