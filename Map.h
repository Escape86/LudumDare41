#pragma once

#include <string>
#include <vector>

#pragma region Forward Declarations
class Texture;
class MapTile;
#pragma endregion

class Map
{
public:
	Map(std::string tileDatafilePath, std::string textureFilepath);
	~Map();

	void Draw();

private:
	void readDataFile(const std::string& tileDatafilePath);

	int rowCount;
	int columnCount;
	Texture* texture;
	std::vector<MapTile*> mapTiles;
};