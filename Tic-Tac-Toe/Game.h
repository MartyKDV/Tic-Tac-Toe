#pragma once
#pragma once
#ifndef __Game__
#define __Game__
#include "SDL.h"
#include <iostream>
#include <string>
using namespace std;
const int SPACE = 20;
enum gamestate {
	selectFirstTurn, playGame
};
enum turnType {
	X, O, TIE, NONE
};
struct button {

	SDL_Rect draw_button;
	SDL_Texture* btnTexture;
	bool pressed;
};
class Game {

public:

	Game() {

	}

	~Game() {

	}

	bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void render();
	void update();
	void handleEvents();
	void clean();

	// BUTTON FUNCTIONS
	void buttonProcessEvent(button *btn, const SDL_Event *event);

	// CHECK FOR WIN CONDITIONS
	turnType checkWinner(const char board[][3]);

	// SWAP THE TURNS
	turnType swapTurn(turnType turn);

	// GET PLAYER'S MOVE
	void getPlayerMove(SDL_Event *event, SDL_Rect node[][3]);

	// GET AI'S MOVE
	void getAIMove(SDL_Event *event, char b[3][3]);

	bool running() {
		return mRunning;
	}

	char board[3][3];

private:

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	SDL_Texture* boardTexture;
	SDL_Texture* xTexture;
	SDL_Texture* oTexture;
	SDL_Texture* emptyTexture;

	SDL_Rect boardSourceRect;
	SDL_Rect boardDestinationRect;
	SDL_Rect nodeSourceRect[3][3];
	SDL_Rect nodeDestinationRect[3][3];

	button xBtn;
	button oBtn;

	turnType turn, playerSign, compSign;

	bool mRunning;
	gamestate gamestate;
};

#endif // !__Game__

