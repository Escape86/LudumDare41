#include "Game.h"
#include "Player.h"
#include "Display.h"
#include "Constants.h"
#include "Audio.h"
#include "Map.h"
#include "Spawn.h"
#include "Teleporter.h"
#include "Enemy.h"
#include "Trigger.h"
#include "MapTile.h"
#include "Texture.h"
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

	this->player = new Player(216.0, 144, Direction::DOWN);
	this->previousFrameEndTime = 0;

	this->initTriggers();

	this->isOnMoon = false;;
	this->isInFantasy = false;
	this->isInWestern = false;
	this->isInSaloon = false;
	this->isInCatHouse = false;

	this->bullTimer = 7.0f;
	this->clearBull = false;

	//load house map initially
	if (!this->SwitchMap("resources/house_interrior.csv", "resources/Interior.png", "resources/house_interrior_spawns.txt", "resources/house_interrior_teleporters.txt"))
	{
#if _DEBUG
	assert(false);	//FAILED TO LOAD MAP!
#endif
		exit(-1);
	}

	this->camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	this->numberOfCowsRescued = 0;

	this->talkingToRed = false;
	this->doneTalkingToRed = false;

	this->heart = new Texture(HEART_TEXTURE_PATH);
	this->heart->Load();

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

	//win condition
	if (this->numberOfCowsRescued >= TOTAL_COWS_TO_RESCUE)
	{
		doChatEvent("YOU SAVED ALL THE COWS!", "THANKS FOR PLAYING!");
		this->numberOfCowsRescued = -999999;
	}

	if (this->talkingToRed)
	{
		if (this->doneTalkingToRed)
		{
			this->teleportToBull();
			this->talkingToRed = false;
		}
	}

	if (this->player)
	{
		//check hp to see if player died
		const int hp = this->player->GetHp();
		if (hp <= 0)
		{
			//player died! send them back to moon spawn
			if (this->isInFantasy)
			{
				this->player->SetPosition(96, 888);
			}
			else
			{
#if _DEBUG
	assert(this->isOnMoon);
#endif
				this->player->SetPosition(96, 1344);
			}

			this->player->SetHp(PLAYER_MAX_HP);
		}

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

	for (std::vector<Spawn*>::iterator it = this->spawns.begin(); it != this->spawns.end();)
	{
		Spawn* spawn = *it;
		spawn->InjectFrame(elapsedTimeInMilliseconds, previousFrameTime);

		//is cow?
		if (this->isInFantasy)
		{
			if (spawn->GetID() == 6 || spawn->GetID() == 7)
			{
				if (this->player->TestCollision(spawn))
				{
					//collected cow!
					delete spawn;
					it = this->spawns.erase(it);
					this->numberOfCowsRescued++;
					continue;
				}
			}
		}
		else if(this->isOnMoon)
		{
			if (spawn->GetID() == 15)
			{
				if (this->player->TestCollision(spawn))
				{
					//collected cow!
					delete spawn;
					it = this->spawns.erase(it);
					this->numberOfCowsRescued++;
					continue;
				}
			}
		}
		else if (this->isInWestern)
		{
			//is bull?
			if (spawn->GetID() == 0)
			{
				if (this->player->TestCollision(spawn))
				{
					double x = this->player->GetPositionX();
					double y = this->player->GetPositionY();
					this->player->SetPosition(x - 10, y - 10);
				}
			}
		}
		

		++it;
	}

	const SDL_Rect* playerAttackHitBox = this->player->GetPlayerAttackHitBox();
	for (std::vector<Enemy*>::iterator it = this->enemies.begin(); it != this->enemies.end();)
	{
		Enemy* enemy = *it;
		enemy->InjectFrame(elapsedTimeInMilliseconds, previousFrameTime);

		//is player currently attacking?
		if (playerAttackHitBox)
		{
			//check if was attacked
			if (enemy->TestCollisionWithRect(playerAttackHitBox))
			{
				enemy->OnHitByPlayerAttack();
				enemy->DoRecoil(this->player->GetFacing());
				Audio::PlayAudio(Audio::AudioTracks::ENEMY_WAS_ATTACKED);
			}
		}

		//did our player and this enemy collide?
		if (enemy->TestCollision(this->player))
		{
			//yup, punish the player!
			int playerHP = this->player->GetHp();
			this->player->SetHp(playerHP - 1);
			enemy->DoRecoil(this->player->GetFacing());
			Audio::PlayAudio(Audio::AudioTracks::PLAYER_WAS_ATTACKED);
		}

		//should enemy die?
		if (enemy->GetHP() <= 0)
		{
			Audio::PlayAudio(Audio::AudioTracks::ENEMY_DIED);

			delete enemy;
			it = this->enemies.erase(it);
		}
		else
		{
			++it;
		}
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

	if (this->isInWestern && this->clearBull)
	{
		float frameTimeInMilliseconds = previousFrameTime / 1000.0f;

		if (this->bullTimer <= 0.0f)
		{
			//remove bull
			for (std::vector<Spawn*>::iterator it = this->spawns.begin(); it != this->spawns.end();)
			{
				Spawn* spawn = *it;

				if (spawn->GetID() == 0)
				{
					delete spawn;
					it = this->spawns.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}

			//turn off flag for event
			this->clearBull = false;
		}
		else
		{
			//move bull out of the way
			for (std::vector<Spawn*>::iterator it = this->spawns.begin(); it != this->spawns.end();)
			{
				Spawn* spawn = *it;

				if (spawn->GetID() == 0)
				{
					double x = spawn->GetPositionX();
					double y = spawn->GetPositionY();
					spawn->SetPosition(x - NPC_VELOCITY * frameTimeInMilliseconds, y);
					break;
				}
			}

			this->bullTimer -= frameTimeInMilliseconds;
		}
	}

	//check triggers
	for (Trigger* trigger : this->currentMapsTriggers)
	{
		trigger->TestAndFireAction(this->player);
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

	//draw hearts ui
	this->drawHeartsUI();

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
			//fake all keys being released
			this->player->OnKeyUp(SDLK_UP);
			this->player->OnKeyUp(SDLK_DOWN);
			this->player->OnKeyUp(SDLK_LEFT);
			this->player->OnKeyUp(SDLK_RIGHT);

			//advance chat or clear chat event if we're done chatting

			//check if queued text
			if (this->queuedTextForChatEvents.size())
			{
				this->doChatEvent(this->queuedTextForChatEvents[0]);
				this->queuedTextForChatEvents.erase(this->queuedTextForChatEvents.begin(), this->queuedTextForChatEvents.begin() + 1);
			}
			else
			{
				//clear and leave chat event
				for (int i = 0; i < 2; i++)
				{
					Display::RemoveText(this->chatTextIds[i]);	//ignore return bool it just means if it found the requested text or not

					if (this->talkingToRed)
					{
						this->doneTalkingToRed = true;
					}
				}
				this->isInChatEvent = false;
			}
		}
		else
		{
			//check if player is colliding with the spawn (may indicate an interaction request)
			bool interaction = false;
			for (Spawn* spawn : this->spawns)
			{
				if (spawn->TestCollision(this->player))
				{
					//yes, do handle the interaction!		
					
					//is it lady in red dress?
					if (this->isInCatHouse && spawn->GetID() == 1)
					{
						//yes!
						this->talkedToWomenInRedDress();
					}
					else
					{
						//nope just normal person
						this->doChatEvent("hi there");
					}
					interaction = true;

					break;
				}
			}

			if (!interaction)
			{
				//nope, just send the message on to the player class and let them handle it (maybe it's combat attacks?)
				this->player->OnKeyDown(key);
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

	this->currentMapsTriggers = this->mapIdToMapTriggers[MapTile::GetMapIdByFileName(mapFilePath)];

	this->isOnMoon = false;
	this->isInFantasy = false;
	this->isInWestern = false;
	this->isInSaloon = false;
	this->isInCatHouse = false;

	if (mapFilePath == "resources/moon.csv")
	{
		this->isOnMoon = true;
	}
	else if (mapFilePath == "resources/fantasy.csv")
	{
		this->isInFantasy = true;
	}
	else if (mapFilePath == "resources/western.csv")
	{
		this->isInWestern = true;
	}
	else if (mapFilePath == "resources/saloon.csv")
	{
		this->isInSaloon = true;
	}
	else if (mapFilePath == "resources/cathouse.csv")
	{
		this->isInCatHouse = true;
	}

	return true;
}

bool Game::GetIsInChatEvent() const
{
	return this->isInChatEvent;
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

	for (Enemy* enemy : this->enemies)
	{
		delete enemy;
	}
	this->enemies.clear();

	if (this->map)
	{
		delete this->map;
		this->map = nullptr;
	}
}
#include<iostream>
void Game::initTriggers()
{
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/western.csv")] =
	{
		new Trigger(15 * TILE_WIDTH, 14 * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, true, [this]()
		{
			this->doChatEvent("Trebek: I better go check the", "corral.");
		}),

		new Trigger(19 * TILE_WIDTH, 28 * TILE_HEIGHT, 12* TILE_WIDTH, TILE_HEIGHT, true, [this]()
		{
			this->doChatEvent("Festus: Trebek! Its awful!");

			this->queuedTextForChatEvents.push_back("The sheriff is looking for you!");
			this->queuedTextForChatEvents.push_back("Go to the saloon!");
		}),

	};
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/bank.csv")] =
	{

	};
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/cathouse.csv")] =
	{
		new Trigger(9 * TILE_WIDTH, 9 * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, true, [this]()
		{
			this->doChatEvent("Bertha: Well if it isn't");

			this->queuedTextForChatEvents.push_back("the great hero Trebek! What");
			this->queuedTextForChatEvents.push_back("brings you to our humble…");
			this->queuedTextForChatEvents.push_back("uh, home ?");

			this->queuedTextForChatEvents.push_back("Trebek : I don't have time for");
			this->queuedTextForChatEvents.push_back("these pleasantries Bertha.");
			this->queuedTextForChatEvents.push_back("My cows are in Jeopardy!");
			this->queuedTextForChatEvents.push_back("No... the town is in Jeopardy.");
			this->queuedTextForChatEvents.push_back("Aliens have been goin' round’");
			this->queuedTextForChatEvents.push_back("stealing the towns' stuff!");

			this->queuedTextForChatEvents.push_back("Bertha : *gasp* Not their STUFF!");
			this->queuedTextForChatEvents.push_back("Well...I know you'll do us right");
			this->queuedTextForChatEvents.push_back("Trebek, you always do.");
			this->queuedTextForChatEvents.push_back("Have you met our new employee,");
			this->queuedTextForChatEvents.push_back("by the way? The one in red.");

			this->queuedTextForChatEvents.push_back("Trebek : BERTHA THAT'S IT!");
			this->queuedTextForChatEvents.push_back("YOU'RE A GENIUS!");
			this->queuedTextForChatEvents.push_back("THE WOMAN IN THE RED DRESS!");
			this->queuedTextForChatEvents.push_back("THE BULL IS GOING TO LOVE HER!");

			this->queuedTextForChatEvents.push_back("Bertha : Wha ? ? ? What are you.. ?");
		}),
	};
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/saloon.csv")] =
	{
		new Trigger(8 * TILE_WIDTH, 8 * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, true, [this]()
		{
			this->doChatEvent("Sheriff Cletus: TREBEK!");

			this->queuedTextForChatEvents.push_back("Thank goodness!");
			this->queuedTextForChatEvents.push_back("The town needs your help.");
			this->queuedTextForChatEvents.push_back("There are all kinds of ");
			this->queuedTextForChatEvents.push_back("science-tific portals in town!");
			this->queuedTextForChatEvents.push_back("People say they've seen aliens,");
			this->queuedTextForChatEvents.push_back("and their stuff is missing!");

			this->queuedTextForChatEvents.push_back("Trebek: That's right sheriff..");
			this->queuedTextForChatEvents.push_back("and they even got my cows.");
			this->queuedTextForChatEvents.push_back("Well I'll set em' straight,");
			this->queuedTextForChatEvents.push_back("I tell you what boy.");
			this->queuedTextForChatEvents.push_back("The portals must be connected!");
			this->queuedTextForChatEvents.push_back("Where can I find them?");

			this->queuedTextForChatEvents.push_back("Sheriff Cletus: Well, one is");
			this->queuedTextForChatEvents.push_back("being guarded by the ol' Bull.");
			this->queuedTextForChatEvents.push_back("He won't take his attention off");
			this->queuedTextForChatEvents.push_back("of it! You'll have to find ");
			this->queuedTextForChatEvents.push_back("a way to distract him! ");
			this->queuedTextForChatEvents.push_back("Another is right over there,");
			this->queuedTextForChatEvents.push_back("in the pisser!");
			this->queuedTextForChatEvents.push_back("So convenient!");
			this->queuedTextForChatEvents.push_back("Seems to be a line though...");
			this->queuedTextForChatEvents.push_back("Trebek: Thanks Sheriff.");
		
		}),
	};
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/house_interrior.csv")] =
	{
		new Trigger(9 * TILE_WIDTH, 6 * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, true, [this]()
		{
			this->doChatEvent("Cow: Moo...Moo...");

			this->queuedTextForChatEvents.push_back("Cow: MOO!!!");

			this->queuedTextForChatEvents.push_back("Trebek: Zz..What in tarnation?!");
			this->queuedTextForChatEvents.push_back("My cows! They need my help! ");
			this->queuedTextForChatEvents.push_back("Sounds like they're in jeopardy!");
		}),
	};
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/moon.csv")] =
	{

	};
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/pisser.csv")] =
	{

	};
	this->mapIdToMapTriggers[MapTile::GetMapIdByFileName("resources/fantasy.csv")] =
	{

	};
}

void Game::doChatEvent(std::string text1, std::string text2 /*=""*/)
{
	//clear any chat
	for (int i = 0; i < 2; i++)
	{
		Display::RemoveText(this->chatTextIds[i]);	//ignore return bool it just means if it found the requested text or not
	}

	if (text1.length())
	{
		this->chatTextIds[0] = Display::CreateText(text1, 12, CHAT_POS_1, Display::TWENTY, true);
	}
	if (text2.length())
	{
		this->chatTextIds[1] = Display::CreateText(text2, 12, CHAT_POS_2, Display::TWENTY, true);
	}

	this->isInChatEvent = true;

	//fake all keys being released
	this->player->OnKeyUp(SDLK_UP);
	this->player->OnKeyUp(SDLK_DOWN);
	this->player->OnKeyUp(SDLK_LEFT);
	this->player->OnKeyUp(SDLK_RIGHT);
}

void Game::talkedToWomenInRedDress()
{
	this->doChatEvent("Trebek: Hey!", "You like Bulls, right ?!");
	this->queuedTextForChatEvents.push_back("Follow me!");
	this->queuedTextForChatEvents.push_back("Red: This is gonna be extra...");

	this->talkingToRed = true;
}

void Game::teleportToBull()
{
	this->teleporters.push_back(new Teleporter(this->player->GetPositionX() - 5, this->player->GetPositionY() - 5, TILE_WIDTH, TILE_HEIGHT, "resources/western.csv", "resources/western.png", "resources/western_spawns.txt", "resources/western_teleporters.txt", 1075.0, 1225.0));

	this->clearBull = true;
}

void Game::drawHeartsUI()
{
	int playerHP = this->player->GetHp();

	int h1Offset = playerHP >= 2 ? 2 : playerHP == 1 ? 1 : 0;
	int h2Offset = playerHP >= 4 ? 2 : playerHP == 3 ? 1 : 0;
	int h3Offset = playerHP >= 6 ? 2 : playerHP == 5 ? 1 : 0;

	//1
	Display::QueueTextureForRendering(this->heart, 5, 5, 20, 20, false, true, h1Offset * 20, 0);
	//2
	Display::QueueTextureForRendering(this->heart, 25, 5, 20, 20, false, true, h2Offset * 20, 0);
	//3
	Display::QueueTextureForRendering(this->heart, 45, 5, 20, 20, false, true, h3Offset * 20, 0);
}

#pragma endregion