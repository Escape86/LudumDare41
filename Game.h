#pragma once
#include "SDL_rect.h"
#include <vector>

#pragma region Forward Declarations
class Player;
class Map;
class Spawn;
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

	const Map* GetMap() const;
	const SDL_Rect& GetCamera() const;

private:
	Player* player;
	Map* map;
	SDL_Rect camera;
	std::vector<Spawn*> spawns;

	unsigned int previousFrameEndTime;

	static Game* _instance;
};