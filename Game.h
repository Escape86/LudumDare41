#pragma once
#include "SDL_rect.h"

#pragma region Forward Declarations
class Player;
class Map;
#pragma endregion

class Game
{
public:
	Game();
	~Game();

	void InjectFrame();
	void InjectKeyDown(int key);
	void InjectKeyUp(int key);
	void InjectControllerStickMovement(unsigned char axis, short value);

private:
	Player* player;
	Map* map;
	SDL_Rect camera;
	unsigned int previousFrameEndTime;
};