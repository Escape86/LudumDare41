#include "Game.h"
#include "Player.h"
#include "Display.h"
#include "Constants.h"
#include "Audio.h"
#include "Map.h"

#pragma region Constructor

Game::Game()
{
	this->player = new Player();
	this->previousFrameEndTime = 0;

	this->map = new Map("resources/test.csv", "resources/test.png");
}

#pragma endregion

#pragma region Public Methods

Game::~Game()
{
	if (this->player)
	{
		delete this->player;
		this->player = nullptr;
	}

	if (this->map)
	{
		delete this->map;
		this->map = nullptr;
	}
}

void Game::InjectFrame()
{
	Uint32 elapsedTimeInMilliseconds = SDL_GetTicks();

	if (this->player)
	{
		//check health for gameover condition
		const int hp = this->player->GetHp();
		if (hp <= 0)
		{
			//player died!
			throw("TODO: HANDLE player died CONDITION HERE!");
			return;
		}
	}

	if (this->player)
	{
		//update player
		this->player->InjectFrame(elapsedTimeInMilliseconds, elapsedTimeInMilliseconds - this->previousFrameEndTime);
	}

	//now that updates are done, draw the frame
	if (this->map)
	{
		this->map->Draw();
	}

	if (this->player)
	{
		//draw player
		this->player->Draw();
	}

	this->previousFrameEndTime = elapsedTimeInMilliseconds;
}

void Game::InjectKeyDown(int key)
{
	if(this->player)
		this->player->OnKeyDown(key);
}

void Game::InjectKeyUp(int key)
{
	if(this->player)
		this->player->OnKeyUp(key);
}

void Game::InjectControllerStickMovement(unsigned char axis, short value)
{
	if (!this->player)
		return;

	//X axis motion
	if (axis == 0)
	{
		//Left of dead zone
		if (value < -JOYSTICK_DEAD_ZONE)
		{
			//fake a keypress for the left direction
			this->player->OnKeyDown(SDLK_LEFT);
		}
		//Right of dead zone
		else if (value > JOYSTICK_DEAD_ZONE)
		{
			//fake a keypress for the right direction
			this->player->OnKeyDown(SDLK_RIGHT);
		}
		else
		{
			this->player->ResetHorizontalVelocity();
		}
	}
	//Y axis motion
	else if (axis == 1)
	{
		//Above of dead zone
		if (value < -JOYSTICK_DEAD_ZONE)
		{
			//fake a keypress for the up direction
			this->player->OnKeyDown(SDLK_UP);
		}
		//Below of dead zone
		else if (value > JOYSTICK_DEAD_ZONE)
		{
			//fake a keypress for the down direction
			this->player->OnKeyDown(SDLK_DOWN);
		}
		else
		{
			this->player->ResetVerticalVelocity();
		}
	}
}

#pragma endregion
