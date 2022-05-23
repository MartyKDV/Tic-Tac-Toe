#include "Game.h"

bool Game::init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen){

	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		mWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);

		if (mWindow != 0) {

			mRenderer = SDL_CreateRenderer(mWindow, -1, 0);

			if (mRenderer != 0) {

				SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
			}
			else {
				cout << "renderer unit fail!";
					return false;
			}
		}
		else {
			cout << "window unit fail!";
				return false;
		}
	}
	else {
		cout << "init failed!";
		return false;
	}

	mRunning = true;
	
	// INIT BOARD and NODES ------------------------------------------------------------------------------------------------------------------------------------------
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {

			nodeSourceRect[i][j].h = nodeDestinationRect[i][j].h = 180;
			nodeSourceRect[i][j].w = nodeDestinationRect[i][j].w = 180;
		}
	}
	SDL_Surface* boardSurface = SDL_LoadBMP("Assets/board.bmp");
	boardTexture = SDL_CreateTextureFromSurface(mRenderer, boardSurface);
	SDL_FreeSurface(boardSurface);
	SDL_QueryTexture(boardTexture, NULL, NULL, &boardSourceRect.w, &boardSourceRect.h);

	SDL_Surface* xSurface = SDL_LoadBMP("Assets/X.bmp");
	xTexture = SDL_CreateTextureFromSurface(mRenderer, xSurface);
	SDL_FreeSurface(xSurface);

	SDL_Surface* oSurface = SDL_LoadBMP("Assets/O.bmp");
	oTexture = SDL_CreateTextureFromSurface(mRenderer, oSurface);
	SDL_FreeSurface(oSurface);

	SDL_Surface* emptySurface = SDL_LoadBMP("Assets/empty.bmp");
	emptyTexture = SDL_CreateTextureFromSurface(mRenderer, emptySurface);
	SDL_FreeSurface(emptySurface);

	boardDestinationRect.x = boardSourceRect.x = 0;
	boardDestinationRect.y = boardSourceRect.y = 0;
	boardDestinationRect.w = boardSourceRect.w;
	boardDestinationRect.h = boardSourceRect.h;

	// INIT BUTTONS ----------------------------------------------------------------------------------------------------------------------------------------
	xBtn.btnTexture = xTexture;
	xBtn.draw_button.x = 140;
	xBtn.draw_button.y = 240;
	xBtn.draw_button.w = 180;
	xBtn.draw_button.h = 180;
	xBtn.pressed = false;

	oBtn.btnTexture = oTexture;
	oBtn.draw_button.x = 345;
	oBtn.draw_button.y = 240;
	oBtn.draw_button.w = 180;
	oBtn.draw_button.h = 180;
	oBtn.pressed = false;

	// INIT BOARD BASE VALUES
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
				
			board[i][j] = ' ';
		}
	}

	gamestate = selectFirstTurn;
	return true;
}

void Game::render() {

	SDL_RenderClear(mRenderer);

	switch (gamestate) {

		case selectFirstTurn: {

			SDL_RenderCopy(mRenderer, xBtn.btnTexture, NULL, &xBtn.draw_button);
			SDL_RenderCopy(mRenderer, oBtn.btnTexture, NULL, &oBtn.draw_button);
		}break;

		case playGame: {

			SDL_RenderCopy(mRenderer, boardTexture, &boardSourceRect, &boardDestinationRect);

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {

					nodeDestinationRect[i][j].x = (j * 180) + SPACE + (j * 2 * SPACE + j * 10);
					nodeDestinationRect[i][j].y = (i * 180) + SPACE + (i * 2 * SPACE + i * 10);
					switch (board[i][j]) {

					case ' ': {

						SDL_RenderCopy(mRenderer, emptyTexture, &nodeSourceRect[i][j], &nodeDestinationRect[i][j]);
					}break;

					case 'X': {

						SDL_RenderCopy(mRenderer, xTexture, &nodeSourceRect[i][j], &nodeDestinationRect[i][j]);
					}break;

					case 'O': {

						SDL_RenderCopy(mRenderer, oTexture, &nodeSourceRect[i][j], &nodeDestinationRect[i][j]);
					}break;
					}
				}
			}

		}break;
	}
	

	SDL_RenderPresent(mRenderer);
}

void Game::update(){

	
	
}

void Game::handleEvents(){

	if (gamestate == playGame) {
		switch (checkWinner(board)) {

		case X: {

			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "GAME OVER", "The Winner is X!", NULL);
			mRunning = false;
		}break;

		case O: {

			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "GAME OVER", "The Winner is O!", NULL);
			mRunning = false;
		}break;

		case TIE: {

			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "GAME OVER", "The Game is a TIE!", NULL);
			mRunning = false;
		}break;

		case NONE: {

		}break;
		}
	}

	SDL_Event event;
	SDL_Event btnEvent;
	SDL_Event inputEvent;
	if (SDL_PollEvent(&event)) {

		switch (event.type) {

			case SDL_QUIT: {
				mRunning = false;
			}break;

			default: {

			}break;
		}
	}

	if (gamestate == selectFirstTurn) {

		SDL_WaitEvent(&btnEvent);
		buttonProcessEvent(&xBtn,&btnEvent);
		buttonProcessEvent(&oBtn,&btnEvent);

		if (xBtn.pressed) {
			turn = X;
			playerSign = X;
			compSign = O;
			gamestate = playGame;
		}
		else {
			if (oBtn.pressed) {
				turn = X;
				playerSign = O;
				compSign = X;
				gamestate = playGame;
			}
		}
	}

	if (gamestate == playGame) {

		if (playerSign == turn) {
			SDL_WaitEvent(&inputEvent);
			getPlayerMove(&inputEvent,nodeDestinationRect);
		}
		else {
			if (compSign == turn) {
				getAIMove(&inputEvent,board);
			}
		}
		
	}
	

}

void Game::clean(){

	cout << "cleaning game\n";
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();
}

void Game::buttonProcessEvent(button * btn, const SDL_Event * event){

	if (event->type == SDL_MOUSEBUTTONDOWN) {
		if (event->button.button == SDL_BUTTON_LEFT &&
			event->button.x >= btn->draw_button.x &&
			event->button.x <= (btn->draw_button.x + btn->draw_button.w) &&
			event->button.y >= btn->draw_button.y &&
			event->button.y <= (btn->draw_button.y + btn->draw_button.h)) {
			
			btn->pressed = true;
		}
	}
}

turnType Game::checkWinner(const char board[][3]){

	char currBoard[9];
	int k = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			currBoard[k] = board[i][j];
			k++;
		}
		
	}
	char WINNING_ROWS[8][3]{{0,1,2},
							{3,4,5},
							{6,7,8},
							{0,3,6},
							{1,4,7},
							{2,5,8},
							{0,4,8},
							{2,4,6}};

	const int TOTAL_ROWS = 8;

	//Check if there is a winner and return it if there is
	for (int row = 0; row < TOTAL_ROWS; row++) {

		if ((currBoard[WINNING_ROWS[row][0]] != ' ') &&
			(currBoard[WINNING_ROWS[row][0]] == currBoard[WINNING_ROWS[row][1]]) &&
			(currBoard[WINNING_ROWS[row][1]] == currBoard[WINNING_ROWS[row][2]])) {

			switch (currBoard[WINNING_ROWS[row][0]]) {

				case 'X': {
					return X;
				}break;

				case 'O': {
					return O;
				}break;
			}

		}

	}

	bool tie = true;
	for (int i = 0; i < 9; i++) {

		if (currBoard[i] == ' ') {
			tie = false;
			break;
		}
	}

	if (tie) {
		return TIE;
	}
	else {
		return NONE;
	}
}

turnType Game::swapTurn(turnType turn){

	if (turn == X) {
		return O;
	}
	else {
		return X;
	}
}

void Game::getPlayerMove(SDL_Event *event, SDL_Rect node[][3]){

	int col = -1, row = -1;
	
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		if (event->button.button == SDL_BUTTON_LEFT) {

			// DETERMINE COLUMN
			if (event->button.x >= node[0][0].x && event->button.x <= node[0][1].x - 50) {

				col = 0;
			}
			if (event->button.x >= node[0][1].x && event->button.x <= node[0][2].x - 50) {

				col = 1;
			}
			if (event->button.x >= node[0][2].x && event->button.x <= 660) {

				col = 2;
			}

			// DETERMINE ROW
			if (event->button.y >= node[0][0].y && event->button.y <= node[1][0].y - 50) {

				row = 0;
			}
			if (event->button.y >= node[1][0].y && event->button.y <= node[2][0].y - 50) {

				row = 1;
			}
			if (event->button.y >= node[2][0].y && event->button.y <= 660) {

				row = 2;
			}
		}

		if (row != -1 && col != -1 && board[row][col] == ' ') {
			switch (playerSign) {

			case X: {
				board[row][col] = 'X';
				turn = swapTurn(turn);
			}break;

			case O: {
				board[row][col] = 'O';
				turn = swapTurn(turn);
			}break;
			}
		}

	}

	
	
}

void Game::getAIMove(SDL_Event * event, char b[3][3]){

	bool moveMade = false;

	// CHECK IF AI CAN WIN. IF IT CAN MAKE THE MOVE
	for (int i = 0; i < 3; i++) {

		if (!moveMade) {
			for (int j = 0; j < 3; j++) {

				if (b[i][j] == ' ') {

					switch (compSign) {

						case X: {
							b[i][j] = 'X';
						}break;

						case O: {
							b[i][j] = 'O';
						}break;
					} 

						if (checkWinner(b) != compSign) {
							b[i][j] = ' ';
						}
						else {
							switch (compSign) {

							case X: {
								board[i][j] = 'X';
							}break;

							case O: {
								board[i][j] = 'O';
							}break;
							}
							moveMade = true;
							break;
						}
				}
			
			}
		}
		
	}

	// CHECK IF THE PLAYER CAN WIN. IF THEY CAN - BLOCK THEM
	for (int i = 0; i < 3; i++) {

		if (!moveMade) {
			for (int j = 0; j < 3; j++) {

				if (b[i][j] == ' ') {

					switch (playerSign) {

						case X: {
							b[i][j] = 'X';
						}break;

						case O: {
							b[i][j] = 'O';
						}break;
					}

						if (checkWinner(b) != playerSign) {
							b[i][j] = ' ';
						}
						else {
							switch (compSign) {
								
								case X: {
									board[i][j] = 'X';
								}break;

								case O: {
									board[i][j] = 'O';
								}break;
							}
							moveMade = true;
							break;
						}
				}

			}
		}

	}


	int BEST_MOVES[9][2] = { {1,1},{0,0},{0,2},{2,0},{2,2},{0,1},{1,0},{1,2},{2,1} };

	for (int i = 0; i < 9; i++) {
		
		if (b[BEST_MOVES[i][0]][BEST_MOVES[i][1]] == ' ' && !moveMade) {

			switch (compSign) {

				case X: {
					board[BEST_MOVES[i][0]][BEST_MOVES[i][1]] = 'X';
				}break;

				case O: {
					board[BEST_MOVES[i][0]][BEST_MOVES[i][1]] = 'O';
				}break;
			}
			moveMade = true;
			break;
		}
	}

	turn = swapTurn(turn);
}
