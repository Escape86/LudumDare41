#pragma once

#include <string>
#include <functional>

#pragma region Forward Declarations
class Object;
#pragma endregion

class Trigger
{
public:
	Trigger(int x, int y, int width, int height, bool runOnce, std::function<void()> action);
	~Trigger();

	bool TestAndFireAction(Object* otherObject);

private:
	int x;
	int y;
	int width;
	int height;
	std::function<void()> action;
	bool runOnce;
	bool completed;
};