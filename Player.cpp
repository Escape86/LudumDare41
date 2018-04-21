#include "Player.h"
#include "Texture.h"
#include "Display.h"
#include "Audio.h"
#include "Constants.h"

const int OverChargeTimerDuration = 1000;

#pragma region Constructor

Player::Player() : Object((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), PLAYER_TEXTURE_PATH)
{
	this->horizontalVelocity = 0;
	this->verticalVelocity = 0;

	this->hp = 100;

	//prevent level switching from causing keyups to occur without a corresponding keydown
	this->keydownPrimed = false;
}

#pragma endregion

#pragma region Public Methods

Player::~Player()
{
}

void Player::InjectFrame(unsigned int elapsedGameTime, unsigned int previousFrameTime)
{
	//update position
	this->x += this->horizontalVelocity;
	this->y += this->verticalVelocity;

	//enforce screen bounds
	int halfWidth = this->width / 2;
	int halfHeight = this->height / 2;

	if (this->x - halfWidth < 0)
	{
		this->x = halfWidth;
	}
	else if (this->x + halfWidth > SCREEN_WIDTH)
	{
		this->x = SCREEN_WIDTH - halfWidth;
	}

	if (this->y - halfHeight < 0)
	{
		this->y = halfHeight;
	}
	else if (this->y + halfHeight > SCREEN_HEIGHT)
	{
		this->y = SCREEN_HEIGHT - halfHeight;
	}
}

void Player::OnKeyDown(int key)
{
	switch (key)
	{
		case SDLK_w:
		case SDLK_UP:
			this->verticalVelocity -= PLAYER_VELOCITY;
			break;
		case SDLK_s:
		case SDLK_DOWN:
			this->verticalVelocity += PLAYER_VELOCITY;
			break;
		case SDLK_a:
		case SDLK_LEFT:
			this->horizontalVelocity -= PLAYER_VELOCITY;
			break;
		case SDLK_d:
		case SDLK_RIGHT:
			this->horizontalVelocity += PLAYER_VELOCITY;
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

#pragma endregion