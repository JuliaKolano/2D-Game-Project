// =======================================================
// SDL Example: Multiple Files
// David Dorrington UoBGames, March 2022
// =======================================================

// Include Elements
#include "GameObjects.h"
#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h> 
#include <stdio.h>
#include<iostream>
using namespace std;
#include<math.h>
#include<string>
#include<SDL_mixer.h>

// Function Prototypes 
void startSDL();
void createLevel();
void playerInputHandler();
void checkCollision();
void updateGameObjects();
void checkGameStates();
void updateGUI();
void updateScreen();
void LevelCompleteScreen();
void limitFrameRate(Uint32);
void closeSDL();

// Game Constants - relaced before compiling
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define SPRITE_SIZE 32
#define FPS 50

// =======================================================
//Global Variables
float frameTime;
SDL_Window* gameWindow = NULL;
SDL_Surface* mainGameSurface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* gameBackground1 = NULL;
SDL_Texture* gameBackground2 = NULL;
SDL_Texture* gameBackground3 = NULL;
SDL_Texture* endScreen = NULL;
SDL_Event playerInputEvent;

// Gobal Text Variables
TTF_Font* font = nullptr;
SDL_Color textColour = { 255, 255, 255 };
SDL_Surface* textSurface = nullptr;
SDL_Texture* textTexture = nullptr;
SDL_Rect textRect;

//Audio
Mix_Music* music = NULL;
Mix_Chunk* arrowSound = NULL;
Mix_Chunk* gameOverSound = NULL;
Mix_Chunk* gameCompleteSound = NULL;
Mix_Chunk* EnemyDieSound = NULL;
Mix_Chunk* PcDamage = NULL;
Mix_Chunk* pickup = NULL;

// game Objects
Character pc;
Projectile bullet;
Object seaweed[20];
Object rocks[120];
Object key;
Object building;
Object chestClosed;
Object chestOpen;
Object message;
Object treasure;
InterObject boulder;
NPC enemies[5];
NPC fish[4];
Object waypoints[12];

//Game Play
bool gamePlaying = true;
bool replay = true;
int npcState = 1;  // Put in class
int airSupply;
bool goalReached = false;
int enemiesLeft = 5;
int currentLevel = 1;

// =======================================================
int main(int argc, char* args[])
{
	Uint32 frameStartTime;

	startSDL();

	//Main Game Loop  
	while (replay)
	{
		createLevel();

		while (gamePlaying)
		{
			frameStartTime = SDL_GetTicks(); // time since starting in milliseconds

			playerInputHandler();
			updateGameObjects();
			checkGameStates();
			updateScreen();

			limitFrameRate(frameStartTime);
		}

		LevelCompleteScreen();
	}
	closeSDL();
	return 0;
}//-----



// =======================================================

int xStart = 64;
int yStart = 64;

void createLevel()
{
	//Create Sprites
	// Background Image Level 1
	const char* spriteImageFile = "images/Underwater.png";
	SDL_Surface* spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
	gameBackground1 = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Background Image Level 2
	spriteImageFile = "images/Village.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
	gameBackground2 = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Background Image Level 3
	spriteImageFile = "images/Battlefield.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
	gameBackground3 = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//End Screen
	spriteImageFile = "images/gameEnd.jpg";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
	endScreen = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Load and Create Sprite Textures
	// PC Sprite 
	spriteImageFile = "images/PC_Idle.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	pc.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	// Bullet 
	spriteImageFile = "images/arrow.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	bullet.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	// Enemies
	spriteImageFile = "images/Orc.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		enemies[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Seaweed
	spriteImageFile = "images/Seaweed.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(seaweed) / sizeof(seaweed[0]); i++)
	{
		seaweed[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//Rocks
	spriteImageFile = "images/Rock.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(rocks) / sizeof(rocks[0]); i++)
	{
		rocks[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//Rocks 2
	if (currentLevel == 2 || currentLevel == 3)
	{
		spriteImageFile = "images/Rock2.png";
		spriteImage = IMG_Load(spriteImageFile);
		if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
		for (int i = 0; i < sizeof(rocks) / sizeof(rocks[0]); i++)
		{
			rocks[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
		}
	}

	//Key
	spriteImageFile = "images/Key.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	key.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Building
	spriteImageFile = "images/Building.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	building.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Boulder
	spriteImageFile = "images/boulder.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	boulder.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Fish
	spriteImageFile = "images/Fish.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(fish) / sizeof(fish[0]); i++)
	{
		fish[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	//Chest Closed
	spriteImageFile = "images/ChestClosed.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	chestClosed.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Chest Open
	spriteImageFile = "images/ChestOpen.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	chestOpen.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Message
	spriteImageFile = "images/Scroll.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	message.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Treasure
	spriteImageFile = "images/Treasure.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	treasure.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Load Audio
	music = Mix_LoadMUS("audio/Woodland Fantasy.wav");
	arrowSound = Mix_LoadWAV("audio/shoot.wav");
	gameOverSound = Mix_LoadWAV("audio/death.wav");
	gameCompleteSound = Mix_LoadWAV("audio/round_end.wav");
	EnemyDieSound = Mix_LoadWAV("audio/Goblin Death.wav");
	PcDamage = Mix_LoadWAV("audio/Damage.wav");
	pickup = Mix_LoadWAV("audio/Accept.wav");


	// Create Levels
	if (currentLevel == 1)
	{
		//Level 1
		//Seaweed
		for (int i = 0; i < sizeof(seaweed) / sizeof(seaweed[0]); i++)
		{
			seaweed[i].isActive = true;
			seaweed[i].x = -30 + i * 50;
			seaweed[i].y = 500;
		}
		//Rocks
		for (int i = 0; i < 30; i++)
		{
			rocks[i].isActive = true;
			rocks[i].x = -30 + i * 30;
			rocks[i].y = 575;
		}
		for (int i = 30, j = 0; i < 50; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = -15;
			rocks[i].y = 40 + j * 30;
		}
		for (int i = 50, j = 0; i < 70; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = 775;
			rocks[i].y = 40 + j * 30;
		}
		for (int i = 70, j = 0; i < 80; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = -30 + j * 30;
			rocks[i].y = 40;
		}
		for (int i = 80, j = 0; i < 95; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = 450 + j * 30;
			rocks[i].y = 40;
		}
		//Fish
		for (int i = 0, j = 0; i < 2; i++, j++)
		{
			fish[i].isAlive = true;
			fish[i].x = 200 + j * 300;
			fish[i].y = 200;
		}
		for (int i = 2, j = 0; i < 4; i++, j++)
		{
			fish[i].isAlive = true;
			fish[i].x = 200 + j * 300;
			fish[i].y = 400;
		}
		//Waypoints
		for (int i = 0, j = 0; i < 2; i++, j++)
		{
			waypoints[i].isActive = true;
			waypoints[i].x = 200 + j * 300;
			waypoints[i].y = 200;
		}
		for (int i = 2, j = 0; i < 4; i++, j++)
		{
			waypoints[i].isActive = true;
			waypoints[i].x = 200 + j * 300;
			waypoints[i].y = 400;
		}
		// PC
		pc.isAlive;
		pc.x = 100;
		pc.y = 200;
		pc.angle = 0;
		//Key
		key.isVisible = true;
		if (key.isVisible)
		{
			key.x = 500;
			key.y = 550;
		}
	}

	//Level 2
	if (currentLevel == 2)
	{
		//Remove Seaweed
		for (int i = 0; i < sizeof(seaweed) / sizeof(seaweed[0]); i++)
		{
			seaweed[i].isActive = false;
		}
		//Remove Fish
		for (int i = 0; i < sizeof(fish) / sizeof(fish[0]); i++)
		{
			fish[i].isAlive = false;
		}
		//Rocks
		for (int i = 0; i < 10; i++)
		{
			rocks[i].isActive = true;
			rocks[i].x = -30 + i * 30;
			rocks[i].y = 575;
		}
		for (int i = 10, j = 0; i < 25; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = 450 + j * 30;
			rocks[i].y = 575;
		}
		for (int i = 25, j = 0; i < 39; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = -15;
			rocks[i].y = 575 - j * 30;
		}
		for (int i = 39, j = 0; i < 53; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = 775;
			rocks[i].y = 575 - j * 30;
		}
		for (int i = 53, j = 0; i < 83; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = 0 + j * 30;
			rocks[i].y = 190;
		}
		//disable extra sprites
		for (int i = 83, j = 0; i < 96; i++, j++)
		{
			rocks[i].isActive = false;
		}
		//PC
		pc.x = 370;
		pc.y = 550;
		//Building
		building.isActive = true;
		building.x = 400;
		building.y = 150;
		//Boulder
		boulder.isActive = true;
		boulder.x = 460;
		boulder.y = 280;
	}

	//Level 3
	if (currentLevel == 3)
	{
		//Remove Boulder
		boulder.isActive = false;
		//Remove Building
		building.isActive = false;
		//Rocks
		for (int i = 0; i < 30; i++)
		{
			rocks[i].isActive = true;
			rocks[i].x = -30 + i * 30;
			rocks[i].y = 575;
		}
		for (int i = 30, j = 0; i < 60; i++, j++)
		{
			rocks[i].isActive = true;
			rocks[i].x = -30 + j * 30;
			rocks[i].y = 190;
		}
		//disable extra sprites
		for (int i = 60, j = 0; i < 83; i++, j++)
		{
			rocks[i].isActive = false;
		}
		//Enemies
		for (int i = 0, j = 0; i < 3; i++, j++)
		{
			enemies[i].isAlive = true;
			enemies[i].x = 200 + j * 250;
			enemies[i].y = 250;
		}
		for (int i = 3, j = 0; i < 6; i++, j++)
		{
			enemies[i].isAlive = true;
			enemies[i].x = 200 + j * 300;
			enemies[i].y = 400;
		}
		//PC
		pc.x = 50;
		pc.y = 300;
		pc.xVel = pc.yVel = 0;
	}
	// Clear surface
	SDL_FreeSurface(spriteImage);
}//-----


// =======================================================
void checkCollision()
{
	SDL_Rect bulletRect = { bullet.x, bullet.y, bullet.size_X, bullet.size_Y };
	SDL_Rect seaweedRect;
	SDL_Rect rockRect;
	SDL_Rect keyRect;
	SDL_Rect buildingRect;
	SDL_Rect boulderRect;
	SDL_Rect fishRect;
	SDL_Rect enemyRect;
	SDL_Rect chestClosedRect;
	SDL_Rect treasureRect;
	SDL_Rect pcRect = { pc.x, pc.y, 75, 90 };

	//Seaweed hit by arrow
	for (int i = 0; i < sizeof(seaweed) / sizeof(seaweed[0]); i++)
	{
		if (bullet.isActive)
		{
			if (seaweed[i].isActive)
			{
				seaweedRect.x = seaweed[i].x;
				seaweedRect.y = seaweed[i].y;
				seaweedRect.h = seaweedRect.w = 100;

				if (SDL_HasIntersection(&bulletRect, &seaweedRect)) // Hit by bullet
				{
					seaweed[i].isActive = false;
					bullet.isActive = false;
				}
			}
		}
	}
	//Enemies hit by arrow
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (bullet.isActive)
		{
			if (enemies[i].isAlive)
			{
				enemyRect.x = enemies[i].x;
				enemyRect.y = enemies[i].y;
				enemyRect.h = enemyRect.w = 50;

				if (SDL_HasIntersection(&bulletRect, &enemyRect)) // Hit by bullet
				{
					enemies[i].isAlive = false;
					bullet.isActive = false;
					enemiesLeft -= 1;
					Mix_PlayChannel(-1, EnemyDieSound, 0);
				}
			}
		}
	}
	
	//rock hit by PC
	for (int i = 0; i < sizeof(rocks) / sizeof(rocks[0]); i++)
	{
		if (rocks[i].isActive)
		{
			rockRect.x = rocks[i].x;
			rockRect.y = rocks[i].y;
			rockRect.h = rockRect.w = 10;

			if (SDL_HasIntersection(&pcRect, &rockRect))
			{
				pc.xVel = -pc.xVel ,pc.yVel = -pc.yVel;
			}
		}
	}

	//key hit by PC
	if (key.isVisible)
	{
		keyRect.x = key.x;
		keyRect.y = key.y;
		keyRect.h = keyRect.w = 1;

		if (SDL_HasIntersection(&pcRect, &keyRect))
		{
			Mix_PlayChannel(-1, pickup, 0);
			key.isVisible = false;
			key.isActive = true;
			pc.hasKey = true;
		}
	}

	//Treasure hit by PC
	if (treasure.isVisible)
	{
		treasureRect.x = treasure.x;
		treasureRect.y = treasure.y;
		treasureRect.h = treasureRect.w = 1;

		if (SDL_HasIntersection(&pcRect, &treasureRect))
		{
			Mix_PlayChannel(-1, pickup, 0);
			treasure.isVisible = false;
			treasure.isActive = true;
			pc.hasTreasure = true;
			goalReached = true;
		}
	}

	//building hit by PC
	if (building.isActive)
	{
		buildingRect.x = building.x + 50;
		buildingRect.y = building.y + 20;
		buildingRect.h = buildingRect.w = 100;

		if (SDL_HasIntersection(&pcRect, &buildingRect))
		{
			pc.xVel = -pc.xVel, pc.yVel = -pc.yVel;
		}
	}

	//PC moves the boulder
	if (boulder.isActive)
	{
		boulderRect.x = boulder.x + 20;
		boulderRect.y = boulder.y;
		boulderRect.h = boulderRect.w = 50;

		if (SDL_HasIntersection(&pcRect, &boulderRect))
		{
			boulder.xVel = pc.xVel + 120;
			if (boulder.xVel > 0) boulder.x += floor(boulder.xVel * frameTime);
			else if (boulder.xVel < 0) boulder.x += ceil(boulder.xVel * frameTime);
		}
	}

	//Fish hits the PC
	for (int i = 0; i < sizeof(fish) / sizeof(fish[0]); i++)
	{
		if (fish[i].isAlive)
		{
			fishRect.x = fish[i].x;
			fishRect.y = fish[i].y;
			fishRect.h = fishRect.w = 50;

			if (SDL_HasIntersection(&pcRect, &fishRect))
			{
				Mix_PlayChannel(-1, PcDamage, 0);
				pc.lives -= 1;
				fish[i].isAlive = false;
			}
		}
	}
	//Enemy hits the PC
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (enemies[i].isAlive)
		{
			enemyRect.x = enemies[i].x;
			enemyRect.y = enemies[i].y;
			enemyRect.h = enemyRect.w = 50;

			if (SDL_HasIntersection(&pcRect, &enemyRect))
			{
				pc.lives -= 1;
				enemiesLeft -= 1;
				enemies[i].isAlive = false;
				Mix_PlayChannel(-1, EnemyDieSound, 0);
				Mix_PlayChannel(-1, PcDamage, 0);
			}
		}
	}
	//PC touches the chest
	if (chestClosed.isActive)
	{
		chestClosedRect.x = chestClosed.x;
		chestClosedRect.y = chestClosed.y;
		chestClosedRect.h = chestClosedRect.w = 100;
		if (SDL_HasIntersection(&pcRect, &chestClosedRect))
		{
			message.isActive = true;
			if (chestOpen.isActive) chestClosed.isActive = false;
		}
		else message.isActive = false;

	}

}//----

// =======================================================
void updateGameObjects()
{
	// Collision Detection ---------------- 
	checkCollision();

	//--------------------------------------
	// Player Character
	pc.screenWrap();
	pc.applyDrag();
	pc.updatePosition(frameTime);

	//enemies
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (enemies[i].isAlive) // exlcude the dead 
		{
			enemies[i].update(pc.x, pc.y, 3);
		}
	}

	//Fish
	for (int i = 0; i < sizeof(fish) / sizeof(fish[0]); i++)
	{
		if (fish[i].isAlive) // exlcude the dead 
		{
			fish[i].update(pc.x, pc.y, 5);
		}
	}

	//----------------------------------
	if (bullet.isActive)
	{
		bullet.checkIsOnScreen();
		bullet.updatePosition(frameTime);
	}

}//-----

void checkGameStates()
{
	if (pc.lives <= 0)
	{
		gamePlaying = false;
		Mix_PlayChannel(-1, gameOverSound, 0);
	}

	if (airSupply <= 0)
	{
		gamePlaying = false;
		Mix_PlayChannel(-1, gameOverSound, 0);
	}

	if (currentLevel == 1)
	{
		if (pc.hasKey == false)
		{
			if (pc.y <= 60) pc.yVel = -pc.yVel;
		}
		if (pc.y <= 0 + SPRITE_SIZE)
		{
			currentLevel = 2;
			createLevel();
		}
	}

	if (currentLevel == 2)
	{
		if (pc.y >= SCREEN_HEIGHT - 50) pc.yVel = -pc.yVel;
		if (boulder.x >= 550 || boulder.x <= 370) boulder.used = true;
		if (boulder.used && (pc.x <= 540 && pc.x >= 420 && pc.y <= 310))
		{
			currentLevel = 3;
			key.isActive = false;
			boulder.isActive = false;
			createLevel();
		}
	}

	if (currentLevel == 3)
	{
		if (enemiesLeft <= 0)
		{
			chestClosed.isActive = true;
			chestClosed.x = 300;
			chestClosed.y = 250;
			if (message.isActive)
			{
				message.x = chestClosed.x - 20;
				message.y = chestClosed.y - 30;
			}
			if (pc.x <= 30) pc.xVel = -pc.xVel;
		}
	}

	if (goalReached)
	{
		Mix_PlayChannel(-1, gameCompleteSound, 0);
		message.isActive = false;
		boulder.isActive = false;
		treasure.isActive = false;
		pc.isAlive = false;
		bullet.isActive = false;
		chestClosed.isActive = false;
		chestOpen.isActive = false;
		for (int i = 0; i < sizeof(rocks) / sizeof(rocks[0]); i++)
		{
			if (rocks[i].isActive) // exlcude the dead 
			{
				rocks[i].isActive = false;
			}
		}
	}
}

// =======================================================
void updateGUI()
{
	string  screenText;

	if (goalReached != true)
	{
		// PC lives
		screenText = " Lives: " + std::to_string(pc.lives);

		textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		int textW = 0;
		int textH = 0;

		SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
		textRect = { 50, 8, textW, textH };
		SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(textTexture);

		//Equipment
		screenText = "Equipment: ";
		textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
		textRect = { 395, 8, textW, textH };
		SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
		if (key.isActive && pc.hasKey)
		{
			key.x = 500;
			key.y = 10;
		}
		if (treasure.isActive && pc.hasTreasure)
		{
			treasure.x = 500;
			treasure.y = 5;
		}

		//Timer
		if (currentLevel == 1)
		{
			airSupply = 33 - SDL_GetTicks() / 1000;
			if (airSupply <= 0) airSupply = 0;
			if (currentLevel != 1) airSupply = 1;
			screenText = "Air Left: " + std::to_string(airSupply);
			textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
			textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
			textRect = { 200, 8, textW, textH };
			SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
		}

		//Message
		if (currentLevel == 3 && message.isActive)
		{
			screenText = "Press E to open";
			textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
			textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
			textRect = { message.x + 6, message.y + 5, textW - 10, textH };
			SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
		}
	}
	
}//-----


// =======================================================
void updateScreen()
{
	//Background Level 1
	SDL_Rect spriteRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT };
	if (currentLevel == 1)
	{
		SDL_Rect spriteRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT };
		// Clear & Update the screen
		SDL_RenderClear(renderer);
		// Set the Background Image
		SDL_RenderCopy(renderer, gameBackground1, NULL, &spriteRect);
	}

	//Background Level 2
	if (currentLevel == 2)
	{
		SDL_Rect spriteRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT };
		// Clear & Update the screen
		SDL_RenderClear(renderer);
		// Set the Background Image
		SDL_RenderCopy(renderer, gameBackground2, NULL, &spriteRect);
	}

	//Background Level 3
	if (currentLevel == 3)
	{
		SDL_Rect spriteRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT };
		// Clear & Update the screen
		SDL_RenderClear(renderer);
		// Set the Background Image
		SDL_RenderCopy(renderer, gameBackground3, NULL, &spriteRect);
	}

	//End Screen
	if (goalReached)
	{
		SDL_Rect spriteRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT };
		// Clear & Update the screen
		SDL_RenderClear(renderer);
		// Set the Background Image
		SDL_RenderCopy(renderer, endScreen, NULL, &spriteRect);
	}

	//Key
	if (key.isVisible)
	{
		spriteRect = { key.x, key.y, 50, 15 };
		SDL_RenderCopyEx(renderer, key.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}
	if (key.isActive)
	{
		spriteRect = { key.x, key.y, 50, 15 };
		SDL_RenderCopyEx(renderer, key.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	// Rocks
	for (int i = 0; i < sizeof(rocks) / sizeof(rocks[0]); i++)
	{
		if (rocks[i].isActive) // exlcude the dead 
		{
			spriteRect = { rocks[i].x, rocks[i].y, 50, 50};
			SDL_RenderCopyEx(renderer, rocks[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	//Bulding
	if (building.isActive)
	{
		spriteRect = { building.x, building.y, 200, 200 };
		SDL_RenderCopyEx(renderer, building.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	//Boulder
	if (boulder.isActive)
	{
		spriteRect = { boulder.x, boulder.y, 100, 100 };
		SDL_RenderCopyEx(renderer, boulder.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	// Seaweed
	for (int i = 0; i < sizeof(seaweed) / sizeof(seaweed[0]); i++)
	{
		if (seaweed[i].isActive) // exlcude the dead 
		{
			spriteRect = { seaweed[i].x, seaweed[i].y, 200 , 100 };
			SDL_RenderCopyEx(renderer, seaweed[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	//Enemies
	for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); i++)
	{
		if (enemies[i].isAlive) // exlcude the dead 
		{
			spriteRect = { enemies[i].x, enemies[i].y, 75, 75 };
			SDL_RenderCopyEx(renderer, enemies[i].spriteTexture, NULL, &spriteRect, enemies[i].angle, NULL, SDL_FLIP_NONE);
		}
	}

	//Fish
	for (int i = 0; i < sizeof(fish) / sizeof(fish[0]); i++)
	{
		if (fish[i].isAlive) // exlcude the dead 
		{
			spriteRect = { fish[i].x, fish[i].y, 50, 50 };
			SDL_RenderCopyEx(renderer, fish[i].spriteTexture, NULL, &spriteRect, fish[i].angle, NULL, SDL_FLIP_NONE);
			fish[i].patrol(waypoints[0].x, waypoints[0].y, waypoints[1].x, waypoints[1].y, waypoints[3].x, waypoints[3].y, waypoints[2].x, waypoints[2].y);
		}
	}

	//Waypoints
	for (int i = 0; i < sizeof(waypoints) / sizeof(waypoints[0]); i++)
	{
		if (waypoints[i].isActive) // exlcude the dead 
		{
			spriteRect = { waypoints[i].x,  waypoints[i].y, 50, 50 };
			SDL_RenderCopy(renderer, waypoints[i].spriteTexture, NULL, &spriteRect);
		}
	}

	//Chest
	if (chestClosed.isActive)
	{
		spriteRect = { chestClosed.x, chestClosed.y, 100, 100 };
		SDL_RenderCopyEx(renderer, chestClosed.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}
	if (chestOpen.isActive)
	{
		spriteRect = { chestOpen.x, chestOpen.y, 100, 100 };
		SDL_RenderCopyEx(renderer, chestOpen.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	//Treasure
	if (treasure.isVisible)
	{
		spriteRect = { treasure.x, treasure.y, 30, 30 };
		SDL_RenderCopyEx(renderer, treasure.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}
	if (treasure.isActive)
	{
		spriteRect = { treasure.x, treasure.y, 30, 30 };
		SDL_RenderCopyEx(renderer, treasure.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	//Message
	if (message.isActive)
	{
		spriteRect = { message.x, message.y, 150, 30 };
		SDL_RenderCopyEx(renderer, message.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	//Arrow
	if (bullet.isActive)
	{
		spriteRect = { bullet.x, bullet.y, bullet.size_X , bullet.size_Y };
		SDL_RenderCopyEx(renderer, bullet.spriteTexture, NULL, &spriteRect, bullet.angle, NULL, SDL_FLIP_NONE);
	}

	//PC
	if (pc.isAlive)
	{
		spriteRect = { pc.x, pc.y, 75 , 90 };
		SDL_RenderCopyEx(renderer, pc.spriteTexture, NULL, &spriteRect, pc.angle, NULL, SDL_FLIP_NONE);
	}
	
	updateGUI();  //******** USER INTERFACE  ***********

	//Update the Screen
	SDL_RenderPresent(renderer);

}//-----


// ======================================================
void playerInputHandler()
{
	// Set up the input event handler
	while (SDL_PollEvent(&playerInputEvent) != 0)
	{
		switch (playerInputEvent.type)
		{
		case SDL_QUIT:
			gamePlaying = false;
			replay = false;
			break;

		case SDL_KEYDOWN:

			switch (playerInputEvent.key.keysym.sym)
			{
			case SDLK_w: pc.moveXY('u');	break;
			case SDLK_s: pc.moveXY('d');	break;
			case SDLK_a: pc.moveXY('l');	break;
			case SDLK_d: pc.moveXY('r');	break;
			case SDLK_SPACE:
			{
				bullet.fire(pc.x + 50, pc.y + 15, pc.angle + 90);
				Mix_PlayChannel(-1, arrowSound, 0); break;
			}
			case SDLK_ESCAPE: gamePlaying = false, replay = false; break;
			case SDLK_1: Mix_PlayMusic(music, -1); break;
			case SDLK_2: Mix_FadeOutMusic(2000);
			if (message.isActive)
			{
				case SDLK_e:
				{
					chestClosed.isActive = false;
					chestOpen.isActive = true;
					treasure.isVisible = true;
					if (treasure.isVisible)
					{
						treasure.x = 350;
						treasure.y = 275;
					}
					chestOpen.x = 300;
					chestOpen.y = 250;
				} break;
			}
			if (goalReached)
			{
				case SDLK_p:
				{
					goalReached = false;
					pc.isAlive = true;
					pc.lives = 3;
					airSupply = 61;
					currentLevel = 1;
					createLevel();
				} break;
			}
				break;
			}
		}
	}
}

 // =======================================================
void LevelCompleteScreen()
{
	gamePlaying = true;
	// Load Title Image
	const char* startImageFile = "images/startScreen.png";
	SDL_Surface* startImage = IMG_Load(startImageFile);

	// merge image with surface
	SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

	SDL_UpdateWindowSurface(gameWindow);

	SDL_Delay(2000);
	SDL_FreeSurface(startImage);
}//-----

 //-----------------------------
void limitFrameRate(Uint32 startTick)
{
	Uint32 currentTick = SDL_GetTicks();

	if ((1000 / FPS) > currentTick - startTick) {
		SDL_Delay(1000 / FPS - (currentTick - startTick));
		frameTime = (float)1 / FPS;
	}
	else
	{
		frameTime = (float)(currentTick - startTick) / 1000;
	}
}//-----

 //-----------------------------
void startSDL()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) printf(" SDL init Error: %s\n", SDL_GetError());
	else
	{	//Create window
		gameWindow = SDL_CreateWindow("CI411 - SDL Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (gameWindow == NULL) printf(" Window Error %s\n", SDL_GetError());
		//Get Game window surface
		mainGameSurface = SDL_GetWindowSurface(gameWindow);

		//Create a renderer for the window 
		renderer = SDL_CreateRenderer(gameWindow, -1, 0);

		//Set the draw color of renderer
		SDL_SetRenderDrawColor(renderer, 125, 125, 125, 0);

		// Initialise Fonts
		TTF_Init();
		font = TTF_OpenFont("fonts/arial.ttf", 20);

		//Audio
		Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	}
}//-----

 //-----------------------------
void closeSDL()
{
	SDL_Delay(1000);
	Mix_FreeChunk(gameCompleteSound);
	Mix_FreeChunk(gameOverSound);
	Mix_FreeChunk(EnemyDieSound);
	Mix_FreeChunk(arrowSound);
	Mix_FreeChunk(PcDamage);
	Mix_FreeChunk(pickup);
	Mix_FreeMusic(music);
	Mix_CloseAudio();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gameWindow);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();

}//-----

 // =======================================================