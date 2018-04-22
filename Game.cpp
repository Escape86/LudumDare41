#include "Game.h"
#include "Player.h"
#include "Display.h"
#include "Constants.h"
#include "Audio.h"
#include "Map.h"
#include "Spawn.h"
#include <fstream>

#if _DEBUG
	#include <assert.h>
#endif

Game* Game::_instance;

#pragma region Constructor

Game::Game()
{

#if _DEBUG
	assert(Game::_instance == nullptr);	//already initialized!
#endif	

	this->player = new Player();
	this->previousFrameEndTime = 0;

	this->map = new Map("resources/test.csv", "resources/test.png");

	bool spawnLoadResult = this->LoadSpawns("resources/spawns.txt");

#if _DEBUG
	assert(spawnLoadResult);
#endif

	this->camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	Game::_instance = this;
}

#pragma endregion

#pragma region Public Methods

Game::~Game()
{
	for (Spawn* spawn : this->spawns)
	{
		delete spawn;
	}
	this->spawns.clear();

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

const Game* Game::GetInstance()
{
	return Game::_instance;
}

void Game::InjectFrame()
{
	Uint32 elapsedTimeInMilliseconds = SDL_GetTicks();

	const unsigned int previousFrameTime = elapsedTimeInMilliseconds - this->previousFrameEndTime;

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

	for (Spawn* spawn : this->spawns)
	{
		spawn->InjectFrame(elapsedTimeInMilliseconds, previousFrameTime);
	}

	if (this->player)
	{
		//update player
		this->player->InjectFrame(elapsedTimeInMilliseconds, previousFrameTime);
	}

	//Center the camera over the dot
	camera.x = (this->player->GetPositionX() + PLAYER_WIDTH / 2) - SCREEN_WIDTH / (2 * RENDER_SCALE_AMOUNT);
	camera.y = (this->player->GetPositionY() + PLAYER_HEIGHT / 2) - SCREEN_HEIGHT / (2 * RENDER_SCALE_AMOUNT);

	//Keep the camera in bounds
	const int mapWidth = this->map->GetColumnCount() * TILE_WIDTH;
	const int mapHeight = this->map->GetRowCount() * TILE_HEIGHT;
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	else if ((camera.x + (camera.w / 2)) > mapWidth)
	{
		camera.x = mapWidth - (camera.w / 2);
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	else if ((camera.y + (camera.h / 2)) > mapHeight)
	{
		camera.y = mapHeight - (camera.h / 2);
	}

	//now that updates are done, draw the frame
	if (this->map)
	{
		this->map->Draw(camera.x, camera.y);
	}

	for (Spawn* spawn : this->spawns)
	{
		spawn->Draw();
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
	if (this->player)
		this->player->OnKeyDown(key);
}

void Game::InjectKeyUp(int key)
{
	if (this->player)
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

bool Game::LoadSpawns(std::string filepath)
{
	std::ifstream file(filepath.c_str());

	if (!file.is_open())
		return false;

	std::string line;
	while (std::getline(file, line))
	{
		if (line.length() == 0)
			continue;

		char* l = _strdup(line.c_str());

		char* context = NULL;

		char* idToken				= strtok_s(l, ",", &context);
		char* spawnXToken			= strtok_s(NULL, ",", &context);
		char* spawnYToken			= strtok_s(NULL, ",", &context);
		char* widthToken			= strtok_s(NULL, ",", &context);
		char* heightToken			= strtok_s(NULL, ",", &context);
		char* texturePathToken		= strtok_s(NULL, ",", &context);
		char* spriteOffsetXToken	= strtok_s(NULL, ",", &context);
		char* spriteOffsetYToken	= strtok_s(NULL, ",", &context);

		if ((idToken			== NULL) ||
			(spawnXToken		== NULL) ||
			(spawnYToken		== NULL) ||
			(widthToken			== NULL) ||
			(heightToken		== NULL) ||
			(texturePathToken	== NULL) ||
			(spriteOffsetXToken == NULL) ||
			(spriteOffsetYToken == NULL))
			return false;

			int id = atoi(idToken);
			double spawnX = atof(spawnXToken);
			double spawnY = atof(spawnYToken);
			int width = atoi(widthToken);
			int height = atoi(heightToken);
			std::string texturePath = texturePathToken;
			int spriteOffsetX = atoi(spriteOffsetXToken);
			int spriteOffsetY = atoi(spriteOffsetYToken);

			//clear whitespace from texturePath
			while (texturePath.size() && isspace(texturePath.front()))	//front
				texturePath.erase(texturePath.begin());
			while (texturePath.size() && isspace(texturePath.back()))	//back
				texturePath.pop_back();

			this->spawns.push_back(new Spawn(id, spawnX, spawnY, width, height, texturePath, spriteOffsetX, spriteOffsetY));

		free(l);
	}

	file.close();

	return true;
}

const Map* Game::GetMap() const
{
	return this->map;
}

const SDL_Rect& Game::GetCamera() const
{
	return this->camera;
}

#pragma endregion
