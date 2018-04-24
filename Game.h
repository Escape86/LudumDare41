#pragma once
#include "Teleporter.h"
#include "SDL_rect.h"
#include <vector>
#include <map>

#pragma region Forward Declarations
class Player;
class Map;
class Spawn;
class Teleporter;
class Enemy;
class Trigger;
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

	bool GetIsInChatEvent() const;

	const Player* GetPlayer() const;

	const Map* GetMap() const;
	const SDL_Rect& GetCamera() const;

private:
	void cleanUpGameObjects();
	void initTriggers();
	void doChatEvent(std::string text1, std::string text2 = "");

	Player* player;
	Map* map;
	SDL_Rect camera;
	std::vector<Spawn*> spawns;
	std::vector<Enemy*> enemies;
	std::vector<Teleporter*> teleporters;
	std::map<int, std::vector<Trigger*>> mapIdToMapTriggers;
	std::vector<Trigger*> currentMapsTriggers;

	bool isInChatEvent;
	int chatTextIds[2] = { -1, -1 };

	Destination destinationMapSwitch;
	bool mapSwitchRequested;

	std::vector<std::string> queuedTextForChatEvents;

	unsigned int previousFrameEndTime;

	bool isOnMoon;
	bool isInFantasy;

	static Game* _instance;
};