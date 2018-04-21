#include "Display.h"
#include "Texture.h"
#include "Constants.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#pragma region Public Methods

bool Display::Initialize()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	//Create window
	Display::window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Create vsynced renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	//Initialize Fonts
	if (!loadFonts())
		return false;

	//check if a JoyStick is present
	if (SDL_NumJoysticks() > 0)
	{
		//Load joystick
		Display::gameController = SDL_JoystickOpen(0);
		if (gameController == nullptr)
		{
			printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}

	//everything initialized correctly!
	return true;
}

bool Display::ShutDown()
{
	//Free loaded images
	for (auto it = Display::textureQueue.begin(); it != Display::textureQueue.end();)
	{
		delete (it->texture);
		it = textureQueue.erase(it);
	}

	//Close game controller
	SDL_JoystickClose(Display::gameController);
	Display::gameController = nullptr;

	//Free fonts
	for (const std::pair<FontSize, TTF_Font*>& font : Display::fonts)
	{
		TTF_CloseFont(font.second);
	}
	Display::fonts.clear();

	//Destroy window	
	SDL_DestroyRenderer(Display::renderer);
	SDL_DestroyWindow(Display::window);
	Display::window = nullptr;
	Display::renderer = nullptr;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();

	return true;
}

void Display::InjectFrame()
{
	//Handle events on queue
	SDL_Event eventHandler;
	while (SDL_PollEvent(&eventHandler) != 0)
	{
		//publish events
		if (Display::eventCallback != nullptr)
		{
			Display::eventCallback(eventHandler);
		}
	}

	//Clear screen
	SDL_SetRenderDrawColor(Display::renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(Display::renderer);

	SDL_SetRenderDrawColor(Display::renderer, 0x00, 0x00, 0x00, 0xFF);

	//draw our queued up textures
	for (std::vector<QueuedTexture>::iterator it = Display::textureQueue.begin(); it != Display::textureQueue.end(); ++it)
	{
		Texture* t = it->texture;
		t->Draw(it->x, it->y);
	}

	Display::textureQueue.clear();

	for (std::vector<QueuedText>::iterator it = Display::textQueue.begin(); it != Display::textQueue.end(); ++it)
	{
		if (!it->isVisible || it->text.empty())
			continue;

		Texture* t = Texture::CreateFromText(it->text, it->textColor, it->fontsize);
		
		if(t)
			t->Draw(it->x, it->y);

		delete t;
	}
	
	//Update screen
	SDL_RenderPresent(Display::renderer);
}

void Display::SetEventCallback(std::function<void(SDL_Event e)> eventCallback)
{
	Display::eventCallback = eventCallback;
}

SDL_Renderer* const Display::GetRenderer()
{
	return Display::renderer;
}

void Display::QueueTextureForRendering(Texture* const texture, int x, int y)
{
	Display::textureQueue.push_back({ texture, x, y });
}

TTF_Font* const Display::GetFont(FontSize size)
{
	return Display::fonts[size];
}

int Display::CreateText(std::string text, int x, int y, Display::FontSize fontSize, SDL_Color textColor /*= { 0, 0, 0 }*/)
{
	int id = Display::textControlIdCounter++;
	Display::textQueue.push_back(QueuedText{ x, y, text, textColor, fontSize, true, id });
	return id;
}

bool Display::UpdateText(int id, std::string text, SDL_Color textColor /*= { 0, 0, 0 }*/)
{
	for (QueuedText& qt : Display::textQueue)
	{
		if (qt.id == id)
		{
			qt.text = text;
			qt.textColor = textColor;
			return true;
		}
	}

	return false;
}

bool Display::MoveText(int id, int x, int y)
{
	for (QueuedText& qt : Display::textQueue)
	{
		if (qt.id == id)
		{
			qt.x = x;
			qt.y = y;
			return true;
		}
	}

	return false;
}

bool Display::SetTextIsVisible(int id, bool isVisible)
{
	for (QueuedText& qt : Display::textQueue)
	{
		if (qt.id == id)
		{
			qt.isVisible = isVisible;
			return true;
		}
	}

	return false;
}

bool Display::RemoveText(int id)
{
	for (std::vector<QueuedText>::iterator it = Display::textQueue.begin(); it != Display::textQueue.end(); ++it)
	{
		if (it->id == id)
		{
			Display::textQueue.erase(it);
			return true;
		}
	}

	return false;
}

#pragma endregion

#pragma region Private Methods
bool Display::loadFonts()
{
	std::vector<FontSize> fontsizesToLoad = { TWENTY, THIRTYFOUR };

	for (int i = 0; i < fontsizesToLoad.size(); i++)
	{
		FontSize size = fontsizesToLoad[i];

		Display::fonts[size] = TTF_OpenFont(FONT_FILEPATH, size);
		if (Display::fonts[size] == nullptr)
		{
			printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
			return false;
		}
	}

	return true;
}
#pragma endregion

#pragma region Static Member Initialization

SDL_Window* Display::window = nullptr;
SDL_Renderer* Display::renderer = nullptr;
std::map<Display::FontSize, TTF_Font*> Display::fonts;
std::function<void(SDL_Event e)> Display::eventCallback;
SDL_Joystick* Display::gameController = nullptr;
std::vector<Display::QueuedTexture> Display::textureQueue;
std::vector<Display::QueuedText> Display::textQueue;
int Display::textControlIdCounter = 0;

#pragma endregion