#pragma once

#include <vector>
#include <functional>
#include <map>
#include "SDL_events.h"
#include "SDL_ttf.h"

#pragma region Forward Declarations
struct SDL_Window;
struct SDL_Renderer;
class Texture;
#pragma endregion

class Display
{
public:
	Display() = delete;

	static bool Initialize();
	static bool ShutDown();
	static void InjectFrame();
	static void SetEventCallback(std::function<void(SDL_Event e)> eventCallback);

	static SDL_Renderer* const GetRenderer();
	static void QueueTextureForRendering(Texture* const texture, int x, int y);

	enum FontSize
	{
		TWENTY		= 20,
		THIRTYFOUR	= 34,

		//remember to add corresponding load functionality when adding new font
	};

	static TTF_Font* const GetFont(FontSize size);

	struct QueuedText
	{
		int x;
		int y;
		std::string text;
		SDL_Color textColor;
		Display::FontSize fontsize;
		bool isVisible;
		int id;
	};

	static int CreateText(std::string text, int x, int y, Display::FontSize fontSize, SDL_Color textColor = { 0, 0, 0 });
	static bool UpdateText(int id, std::string text, SDL_Color textColor = { 0, 0, 0 });
	static bool MoveText(int id, int x, int y);
	static bool SetTextIsVisible(int id, bool isVisible);
	static bool RemoveText(int id);

private:
	static bool loadFonts();

	static SDL_Window* window;
	static SDL_Renderer* renderer;
	static std::map<FontSize, TTF_Font*> fonts;
	static std::function<void(SDL_Event e)> eventCallback;
	static SDL_Joystick* gameController;
	static int textControlIdCounter;

	struct QueuedTexture 
	{
		Texture* texture;
		int x;
		int y;
	};
	static std::vector<QueuedTexture> textureQueue;

	static std::vector<QueuedText> textQueue;
};