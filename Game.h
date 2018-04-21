#pragma once

#pragma region Forward Declarations
class Player;
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

	unsigned int previousFrameEndTime;
};