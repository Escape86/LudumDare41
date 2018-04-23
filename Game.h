#pragma once
#include "Teleporter.h"
#include "SDL_rect.h"
#include <vector>

#pragma region Forward Declarations
class Player;
class Map;
class Spawn;
class Teleporter;
class Enemy;
#pragma endregion

class Game
{
public:
	Game();
	~Game();

	static const Game* GetInstance();

	void InjectFrame();
	void InjectKeyDown(int key);
	void InjectKeyUp(int key);
	void InjectControllerStickMovement(unsigned char axis, short value);
	
	bool LoadSpawns(std::string filepath);

	bool LoadTeleporters(std::string filepath);

	bool SwitchMap(std::string mapFilePath, std::string mapTextureFilePath, std::string spawnsFilePath, std::string teleportersFilePath);

	const Player* GetPlayer() const;

	const Map* GetMap() const;
	const SDL_Rect& GetCamera() const;

private:
	void cleanUpGameObjects();

	Player* player;
	Map* map;
	SDL_Rect camera;
	std::vector<Spawn*> spawns;
	std::vector<Enemy*> enemies;
	std::vector<Teleporter*> teleporters;

	bool isInChatEvent;
	int chatTextIds[2] = { -1, -1 };

	Destination destinationMapSwitch;
	bool mapSwitchRequested;

	unsigned int previousFrameEndTime;

	static Game* _instance;
};