#include "Game.h"
#include "Player.h"
#include "Display.h"
#include "Constants.h"
#include "Audio.h"
#include "Map.h"
#include "Spawn.h"
#include "Teleporter.h"
#include "Enemy.h"
#include <fstream>
#include <algorithm>
#include <time.h>

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

	//load western map initially
	if (!this->SwitchMap("resources/western.csv", "resources/western.png", "resources/western_spawns.txt", "resources/western_teleporters.txt"))
	{
#if _DEBUG
	assert(false);	//FAILED TO LOAD MAP!
#endif
		exit(-1);
	}

	this->camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	Game::_instance = this;

	srand(time(NULL));	//seed our random number generation
}

#pragma endregion

#pragma region Public Methods

Game::~Game()
{
	this->cleanUpGameObjects();

	if (this->player)
	{
		delete this->player;
		this->player = nullptr;
	}
}

const Game* Game::GetInstance()
{
	return Game::_instance;
}

void Game::InjectFrame()
{
	Uint32 elapsedTimeInMilliseconds = SDL_GetTicks();

	if (this->mapSwitchRequested)
	{
		const Destination& destination = this->destinationMapSwitch;
		if (this->SwitchMap(destination.destinationMapFilePath, destination.destinationMapTextureFilePath, destination.destinationSpawnsFilePath, destination.destinationTeleportersFilePath))
		{
			//move player to requested location in new map
			this->player->SetPosition(destination.destinationX, destination.destinationY);

			this->mapSwitchRequested = false;
		}

		return;
	}

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

	if (this->player)
	{
		//update player
		this->player->InjectFrame(elapsedTimeInMilliseconds, previousFrameTime);

		//check if in teleporter
		for (const Teleporter* tp : this->teleporters)
		{
			if (tp->TestCollision(this->player))
			{
				this->destinationMapSwitch = tp->GetDestination();
				this->mapSwitchRequested = true;
			}
		}
	}

	for (Spawn* spawn : this->spawns)
	{
		spawn->InjectFrame(elapsedTimeInMilliseconds, previousFrameTime);
	}

	for (Enemy* enemy : this->enemies)
	{
		enemy->InjectFrame(elapsedTimeInMilliseconds, previousFrameTime);
	}

	//Center the camera over the player
	camera.x = (this->player->GetPositionX() + PLAYER_WIDTH / 2) - SCREEN_WIDTH / (2 * RENDER_SCALE_AMOUNT);
	camera.y = (this->player->GetPositionY() + PLAYER_HEIGHT / 2) - SCREEN_HEIGHT / (2 * RENDER_SCALE_AMOUNT);

	//Keep the camera in bounds
	const int mapWidth = this->map->GetColumnCount() * TILE_WIDTH;
	const int mapHeight = this->map->GetRowCount() * TILE_HEIGHT;
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	else if ((camera.x + (camera.w / 2)) > (mapWidth - (TILE_WIDTH / 2)))
	{
		camera.x = mapWidth - (camera.w / 2) - (TILE_WIDTH / 2);
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	else if ((camera.y + (camera.h / 2)) > (mapHeight - (TILE_HEIGHT / 2)))
	{
		camera.y = mapHeight - (camera.h / 2) - (TILE_HEIGHT / 2);
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

	for (Enemy* enemy : this->enemies)
	{
		enemy->Draw();
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
	//don't do any actions if we don't even have a player spawned
	if (!this->player)
		return;

	//ignore player movement while in the middle of a chat event
	if (!this->isInChatEvent)
	{
		this->player->OnKeyDown(key);
	}

	//spacebar means action button was pressed
	if (key == 32)
	{
		//are we currently chatting?
		if (this->isInChatEvent)
		{
			//advance chat or clear chat event if we're done chatting
			for (int i = 0; i < 2; i++)
			{
				Display::RemoveText(this->chatTextIds[i]);	//ignore return bool it just means if it found the requested text or not
			}
			this->isInChatEvent = false;
		}
		else
		{
			//check if player is colliding with the spawn (may indicate an interaction request)
			for (Spawn* spawn : this->spawns)
			{
				if (spawn->TestCollision(this->player))
				{
					//yes, do handle the interaction!
					this->chatTextIds[0] = Display::CreateText("hi there", 12, CHAT_POS_1, Display::TWENTY, true);
					this->isInChatEvent = true;

					//fake all keys being released
					this->player->OnKeyUp(SDLK_UP);
					this->player->OnKeyUp(SDLK_DOWN);
					this->player->OnKeyUp(SDLK_LEFT);
					this->player->OnKeyUp(SDLK_RIGHT);
					break;
				}
			}
		}
	}
}

void Game::InjectKeyUp(int key)
{
	if (!this->player)
		return;

	//ignore player movement while in the middle of a chat event
	if (!this->isInChatEvent)
	{
		this->player->OnKeyUp(key);
	}
}

void Game::InjectControllerStickMovement(unsigned char axis, short value)
{
	if (!this->player)
		return;

	//ignore player movement while in the middle of a chat event
	if (this->isInChatEvent)
	{
		return;
	}

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
		if (line.length() < 2)
			continue;

		//skip comment lines
		if (line[0] == '-' && line[1] == '-')
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
		char* shouldIdleMoveToken	= strtok_s(NULL, ",", &context);
		char* isEnemyToken			= strtok_s(NULL, ",", &context);

		if ((idToken				== NULL) ||
			(spawnXToken			== NULL) ||
			(spawnYToken			== NULL) ||
			(widthToken				== NULL) ||
			(heightToken			== NULL) ||
			(texturePathToken		== NULL) ||
			(spriteOffsetXToken		== NULL) ||
			(spriteOffsetYToken		== NULL) ||
			(shouldIdleMoveToken	== NULL) ||
			(isEnemyToken			== NULL))
			return false;

			int id = atoi(idToken);
			double spawnX = atof(spawnXToken);
			double spawnY = atof(spawnYToken);
			int width = atoi(widthToken);
			int height = atoi(heightToken);
			std::string texturePath = texturePathToken;
			int spriteOffsetX = atoi(spriteOffsetXToken);
			int spriteOffsetY = atoi(spriteOffsetYToken);

			std::string shouldIdleMoveAsString(shouldIdleMoveToken);
			//clear whitespace from shouldIdleMoveAsString
			while (shouldIdleMoveAsString.size() && isspace(shouldIdleMoveAsString.front()))	//front
				shouldIdleMoveAsString.erase(shouldIdleMoveAsString.begin());
			while (shouldIdleMoveAsString.size() && isspace(shouldIdleMoveAsString.back()))	//back
				shouldIdleMoveAsString.pop_back();
			std::transform(shouldIdleMoveAsString.begin(), shouldIdleMoveAsString.end(), shouldIdleMoveAsString.begin(), ::tolower);
			bool shouldIdleMove = shouldIdleMoveAsString.compare("true") == 0;

			std::string isEnemyAsString(isEnemyToken);
			//clear whitespace from isEnemyAsString
			while (isEnemyAsString.size() && isspace(isEnemyAsString.front()))	//front
				isEnemyAsString.erase(isEnemyAsString.begin());
			while (isEnemyAsString.size() && isspace(isEnemyAsString.back()))	//back
				isEnemyAsString.pop_back();
			std::transform(isEnemyAsString.begin(), isEnemyAsString.end(), isEnemyAsString.begin(), ::tolower);
			bool isEnemy = isEnemyAsString.compare("true") == 0;

			//clear whitespace from texturePath
			while (texturePath.size() && isspace(texturePath.front()))	//front
				texturePath.erase(texturePath.begin());
			while (texturePath.size() && isspace(texturePath.back()))	//back
				texturePath.pop_back();

			if (isEnemy)
			{
				this->enemies.push_back(new Enemy(id, spawnX, spawnY, width, height, texturePath, spriteOffsetX, spriteOffsetY, shouldIdleMove));
			}
			else
			{
				this->spawns.push_back(new Spawn(id, spawnX, spawnY, width, height, texturePath, spriteOffsetX, spriteOffsetY, shouldIdleMove));
			}
			
		free(l);
	}

	file.close();

	return true;
}

bool Game::LoadTeleporters(std::string filepath)
{
	std::ifstream file(filepath.c_str());

	if (!file.is_open())
		return false;

	std::string line;
	while (std::getline(file, line))
	{
		if (line.length() < 2)
			continue;

		//skip comment lines
		if (line[0] == '-' && line[1] == '-')
			continue;

		char* l = _strdup(line.c_str());

		char* context = NULL;

		char* xPosToken								= strtok_s(l, ",", &context);
		char* yPosToken								= strtok_s(NULL, ",", &context);
		char* destinationMapFilePathToken			= strtok_s(NULL, ",", &context);
		char* destinationMapTextureFilePathToken	= strtok_s(NULL, ",", &context);
		char* destinationSpawnsFilePathToken		= strtok_s(NULL, ",", &context);
		char* destinationTeleportersFilePathToken	= strtok_s(NULL, ",", &context);
		char* destinationXToken						= strtok_s(NULL, ",", &context);
		char* destinationYToken						= strtok_s(NULL, ",", &context);

		if ((xPosToken								== NULL) ||
			(yPosToken								== NULL) ||
			(destinationMapFilePathToken			== NULL) ||
			(destinationMapTextureFilePathToken		== NULL) ||
			(destinationSpawnsFilePathToken			== NULL) ||
			(destinationTeleportersFilePathToken	== NULL) ||
			(destinationXToken						== NULL) ||
			(destinationYToken						== NULL))
			return false;

		int xPos = atoi(xPosToken);
		int yPos = atof(yPosToken);
		std::string destinationMapFilePath = destinationMapFilePathToken;
		std::string destinationMapTextureFilePath = destinationMapTextureFilePathToken;
		std::string destinationSpawnsFilePath = destinationSpawnsFilePathToken;
		std::string destinationTeleportersFilePath = destinationTeleportersFilePathToken;
		int destinationX = atoi(destinationXToken);
		int destinationY = atoi(destinationYToken);

		//clear whitespace from destinationMapFilePath
		while (destinationMapFilePath.size() && isspace(destinationMapFilePath.front()))	//front
			destinationMapFilePath.erase(destinationMapFilePath.begin());
		while (destinationMapFilePath.size() && isspace(destinationMapFilePath.back()))	//back
			destinationMapFilePath.pop_back();

		//clear whitespace from destinationMapTextureFilePath
		while (destinationMapTextureFilePath.size() && isspace(destinationMapTextureFilePath.front()))	//front
			destinationMapTextureFilePath.erase(destinationMapTextureFilePath.begin());
		while (destinationMapTextureFilePath.size() && isspace(destinationMapTextureFilePath.back()))	//back
			destinationMapTextureFilePath.pop_back();

		//clear whitespace from destinationSpawnsFilePath
		while (destinationSpawnsFilePath.size() && isspace(destinationSpawnsFilePath.front()))	//front
			destinationSpawnsFilePath.erase(destinationSpawnsFilePath.begin());
		while (destinationSpawnsFilePath.size() && isspace(destinationSpawnsFilePath.back()))	//back
			destinationSpawnsFilePath.pop_back();

		//clear whitespace from destinationTeleportersFilePath
		while (destinationTeleportersFilePath.size() && isspace(destinationTeleportersFilePath.front()))	//front
			destinationTeleportersFilePath.erase(destinationTeleportersFilePath.begin());
		while (destinationTeleportersFilePath.size() && isspace(destinationTeleportersFilePath.back()))	//back
			destinationTeleportersFilePath.pop_back();

		this->teleporters.push_back(new Teleporter(xPos, yPos, TELEPORTER_WIDTH, TELEPORTER_HEIGHT, destinationMapFilePath, destinationMapTextureFilePath, destinationSpawnsFilePath, destinationTeleportersFilePath, destinationX, destinationY));

		free(l);
	}

	file.close();

	return true;
}

bool Game::SwitchMap(std::string mapFilePath, std::string mapTextureFilePath, std::string spawnsFilePath, std::string teleportersFilePath)
{
	//nuke any existing map stuff we have loaded so we can make a fresh start (and not leak memory)
	this->cleanUpGameObjects();

	this->map = new Map(mapFilePath, mapTextureFilePath);

	if (!map)
		return false;

	bool spawnLoadResult = this->LoadSpawns(spawnsFilePath);

#if _DEBUG
	assert(spawnLoadResult);
#endif

	if (!spawnLoadResult)
		return false;

	bool loadTeleportersResult = this->LoadTeleporters(teleportersFilePath);

#if _DEBUG
	assert(loadTeleportersResult);
#endif

	if (!loadTeleportersResult)
		return false;

	return true;
}

const Player* Game::GetPlayer() const
{
	return this->player;
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

#pragma region Private Methods

void Game::cleanUpGameObjects()
{
	for (Teleporter* tp : this->teleporters)
	{
		delete tp;
	}
	this->teleporters.clear();

	for (Spawn* spawn : this->spawns)
	{
		delete spawn;
	}
	this->spawns.clear();

	if (this->map)
	{
		delete this->map;
		this->map = nullptr;
	}
}

#pragma endregion