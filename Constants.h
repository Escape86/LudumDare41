#pragma once

#pragma region Filepaths
#define PLAYER_TEXTURE_PATH				"resources/player.png"
#define FONT_FILEPATH					"resources/Fonts/Orbitron/Orbitron-Regular.ttf"
#pragma endregion

#pragma region Colors
#define BLUE							{ 0, 0, 255 }
#define GREEN							{ 0, 255, 0 }
#define RED								{ 255, 0, 0 }
#define BLACK							{ 0, 0, 0 }
#define WHITE							{ 255, 255, 255}
#pragma endregion

#define RENDER_SCALE_AMOUNT				2.0f	//if you change this you'll need to change the values in Game.cpp that enforce camera bounds

#define SCREEN_WIDTH					800
#define SCREEN_HEIGHT					600

#define TILE_WIDTH						24
#define TILE_HEIGHT						24

#define TELEPORTER_WIDTH				TILE_WIDTH
#define TELEPORTER_HEIGHT				1		//1 so you have to slightly walk into the door/portal area to actually be teleported

#define PLAYER_WIDTH					24
#define PLAYER_HEIGHT					24

#define PLAYER_VELOCITY					275
#define PLAYER_ANIMATION_COOLDOWN		85
#define NPC_VELOCITY					50
#define NPC_IDLEMOVEMENT_COOLDOWN		1200	//in milliseconds

#define ENEMY_VELOCITY					50

#define CHAT_POS_1						SCREEN_HEIGHT * .41 + 5
#define CHAT_POS_2						SCREEN_HEIGHT * .45 + 4

#define JOYSTICK_DEAD_ZONE				8000

#define WINDOW_TITLE					"Trebek's Alien RoundUp"