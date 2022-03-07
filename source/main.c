#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "state.h"

#include "components/menu.h"
#include "components/game.h"
#include "components/editor.h"

#include <3ds.h>
#include <citro2d.h>

int main(int argc, char const *argv[]) {

	//Init libs
	gfxInitDefault();
	if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
		// error
		;
	if (!C2D_Init(C2D_DEFAULT_MAX_OBJECTS * 2))
		// error
		;
	
	C2D_Prepare();

	//Initiate romfs
	Result rc = romfsInit();

	if (rc) {
		// error
		exit(1);
	}

	GlobalState state = { 
		.paused=0,
		.quit=0,
		.framesSinceKeyPress=0,
		.gameState=GS_MENU,
		.top=C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT),
		.bottom=C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT),
		.kDown=0,
		.kUp=0,
		.kHeld=0,
		.preserveBottom=0,
		.cellSize=4.0f,
	};

	//consoleInit(C2D_BotLeft, NULL);

	//Generate strings for main menu
	Menu *mainMenu = initMenu();
	if (mainMenu == NULL)
		// error
		;

	Game *game = initGame(NULL, state.cellSize);
	if (game == NULL)
		// error
		;

	Editor *editor = initEditor(NULL, state.cellSize);
	if (editor == NULL)
		// error
		;

	//Main loop
	while (aptMainLoop()) {
		enum GameState old = state.gameState;

		//If quit = 1 then quit the game loop
		if (state.quit) break;

		//Read button and touchscreen inputs
		updateGlobalState(&state);

		switch (state.gameState) {
			case GS_MENU:
				if (doMenuFrame(mainMenu, &state))
					// error
					;
				break;

				
			case GS_GAME:
				if (doGameFrame(game, &state))
					// error
					;
				break;

			case GS_EDITOR:
				if (doEditorFrame(editor, &state))
					// error
					;
				break;

			case GS_CONSOLE:

				break;

			case GS_TOTAL:
			default:
				break;
		}

		// if the gameState has changed...
		if (old != state.gameState) {
			// Deinit the old state and init the new state
			switch (state.gameState) {
				case GS_MENU:
					mainMenu = initMenu();
					if (mainMenu == NULL)
						// error
						;
					break;
				
				case GS_GAME:
					game = initGame(game, 0);
					if (game == NULL)
						// error
						;
					
					if (old == GS_EDITOR) {
						// If previous state was editor
						// Set the game grid to the editor grid
						// And pause the game by default
						setGameGrid(game, editor->grid);
						game->paused = 1;
					}
					break;

				case GS_EDITOR:
					editor = initEditor(editor, state.cellSize);
					if (editor == NULL)
						// error
						;
					break;

				case GS_CONSOLE:
					break;

				case GS_TOTAL:
				default:
					break;
			}

			switch (old) {
				case GS_MENU:
					destroyMenu(mainMenu);
					break;
				
				case GS_GAME:
					destroyGame(game);
					break;

				case GS_EDITOR:
					destroyEditor(editor);
					break;

				case GS_CONSOLE:
					break;

				case GS_TOTAL:
				default:
					break;
			}
		}

		//Reset counter if key is pressed or touch screen is touched
		if (state.gameState == GS_MENU && (state.kDown || (state.touch.px || state.touch.py))) {
			state.framesSinceKeyPress = 0;
		}
	}

end:

	//If any pointers are still initialised, free them
	destroyMenu(mainMenu);
	destroyFullyGame(game);
	destroyFullyEditor(editor);

	//Deinit libs
	C2D_Fini();
	C3D_Fini();
	romfsExit();
	gfxExit();

	return 0;
}