#include "Game.h"

Game* game = 0;
int main(int argc, char* args[]) {

	game = new Game();
	game->init("Tic-Tac-Toe", 100, 100, 680, 680, false);

	while (game->running()) {

		game->handleEvents();
		game->update();
		game->render();
	}

	game->clean();

	return 0;
}