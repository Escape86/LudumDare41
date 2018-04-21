#pragma once

#include "SDL.h"
#include "Display.h"
#include <string>


class Texture
{
public:
	Texture(const std::string path);
	~Texture();

	static Texture* CreateFromText(const std::string text, SDL_Color textColor, Display::FontSize fontSize);

	bool Load();
	void Draw(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

	int GetWidth();
	int GetHeight();

private:
	Texture();	//for use with CreateFromText()

	bool isLoaded;
	int width;
	int height;
	std::string path;
	bool isForText;

	SDL_Texture* sdl_texture;
};