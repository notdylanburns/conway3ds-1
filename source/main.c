#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "grid.h"

#include <3ds.h>
#include <citro2d.h>

void getTitle(Grid *grid);
void draw(Grid *grid, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour);
void updateGrid(Grid *grid);
void drawMenu(C3D_RenderTarget *screen, char selection, u32 bgColour, u32 fgColour);
void beginFrame();
void endFrame();
void clrScreen(C3D_RenderTarget *screen, u32 colour);
void mainMenuInit();
void mainMenuDeInit();

//Create text objects for menu
C2D_TextBuf menuBuffer;
C2D_Text menuText[3];

//Create text objects for game
C2D_TextBuf gameBuffer;
C2D_Text gameText[1];

int main(int argc, char const *argv[])
{

	//Init libs
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	//Initiate romfs
	Result rc = romfsInit();

	if (rc) {
		exit(1);
	}

	//Create screens
	C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	//Define Colours
	u32 clrWhite = C2D_Color32f(1.0f,1.0f,1.0f,1.0f);
	u32 clrBlack = C2D_Color32f(0.0f,0.0f,0.0f,1.0f);
	//u32 clrGrey = C2D_Color32f(211.0f,211.0f,211.0f,1.0f);
	u32 clrRed = C2D_Color32f(1.0f,0.0f,0.0f,1.0f);

	//Set game state
	signed char gameState = 0; //0=menu 1=game 2=editor

	//Set menu selection
	signed char menuSelection = 0; //0=START 1=EDITOR 2=EXIT

	//Set game paused status
	unsigned char gamePaused = 0;
	float pausetextWidth, pausetextHeight, pausetextPadding, pausetextX, pausetextY;

	//Generate strings for main menu
	mainMenuInit();

	//Set boolean for exiting game
	char quit = 0;

	//Create counter for frames since a key was pressed
	uint32_t framesSinceKeyPress = 0;

	//Create touch position object for touchscreen pressed
	touchPosition touch;

	//Create Empty Grid for titlescreen
	Grid *titlescreen = newEmptyGrid(4.0f);

	//Create empty grid pointer for game
	Grid *grid;

	//Create empty grid pointer for editor
	Grid *editor;
	uint32_t cellCursor = 0;

	//Main loop
	while (aptMainLoop()) {

		//If quit = 1 then quit the game loop
		if (quit) break;

		//Read button and touchscreen inputs
		hidScanInput();
		hidTouchRead(&touch);

		u32 kDown = hidKeysDown();

		switch(gameState) {
			case 0:

				if (framesSinceKeyPress > 1200) {
					updateGrid(titlescreen);
					framesSinceKeyPress = 1200; //Keep framesincekeypress at 1200 so as to prevent overflows if the game is left too long
				} else if (framesSinceKeyPress == 0) {
					getTitle(titlescreen);
				}

				//Menu Navigation
				if (kDown & KEY_DDOWN) {
					menuSelection += 1;
					if (menuSelection > 2) menuSelection = 0;
				}
				if (kDown & KEY_DUP) {
					menuSelection -= 1;
					if (menuSelection < 0) menuSelection = 2;
				}

				//Menu Logic
				if (kDown & KEY_A) {
					switch(menuSelection) {
						case 0:
							//When switching to game, free the main menu from memory and initialise the grid
							destroyGrid(titlescreen);
							titlescreen = NULL;
							grid = newEmptyGrid(4.0f);
							fillGridRandom(grid);
							framesSinceKeyPress = 0;
							gameState = 1;

							//Create game text buffers
							gameBuffer = C2D_TextBufNew(7);
							C2D_TextParse(&gameText[0], gameBuffer, "PAUSED");
							C2D_TextOptimize(&gameText[0]);
							pausetextPadding = 5.0f;
							C2D_TextGetDimensions(&gameText[0], 1.0f, 1.0f, &pausetextWidth, &pausetextHeight);
							pausetextX = TOP_SCREEN_WIDTH - pausetextWidth - pausetextPadding;
							pausetextY = pausetextPadding;
							break;
						case 1:
							//When switching to editor, free the main menu from memory and initialise the grid
							destroyGrid(titlescreen);
							titlescreen = NULL;
							editor = newEmptyGrid(4.0f);
							framesSinceKeyPress = 0;
							gameState = 2;
							break;
						case 2:
							quit = 1;
							break;
					}
				}

				//Check for exit signal
				if (gameState == 1) {
					break;
				}

				beginFrame();
				//Draw Title Screen
				draw(titlescreen, top, clrBlack, clrWhite);
				//Draw Main Menu
				drawMenu(bottom, menuSelection, clrBlack, clrWhite);
				endFrame();
				framesSinceKeyPress += 1;
				break;
			case 1:
				//Randomise grid when a button is pressed
				if (kDown & KEY_A) {
					fillGridRandom(grid);
					beginFrame();
					draw(grid, top, clrBlack, clrWhite);
					//Clear bottom screen
					C2D_SceneBegin(bottom);
					clrScreen(bottom, clrBlack);
					endFrame();
					break;
				}

				//If start button is pressed, pause game
				if (kDown & KEY_START) {
					gamePaused = !gamePaused;
				}

				//If select button is pressed, go back to main menu
				if (kDown & KEY_SELECT) {
					//When switching to the main menu free the grid from memory and reinitialise the title
					destroyGrid(grid);
					grid = NULL;
					titlescreen = newEmptyGrid(4.0f);
					//Delete text buffer
					C2D_TextBufDelete(gameBuffer);
					//Set game state and menu selection and unpause game
					gamePaused = 0;
					menuSelection = 0;
					gameState = 0;
					break;
				}

				//Update screen if the game isn't paused
				if (!gamePaused) {
					//Update cells then draw
					updateGrid(grid);
					beginFrame();
					draw(grid, top, clrBlack, clrWhite);
					//Clear bottom screen
					C2D_SceneBegin(bottom);
					clrScreen(bottom, clrBlack);
					endFrame();
				} else {
					beginFrame();
					C2D_SceneBegin(top);
					C2D_DrawText(&gameText[0],C2D_WithColor, pausetextX, pausetextY, 0.0f, 1.0f, 1.0f, clrRed);
					endFrame();
				}
				break;
			case 2:

				uint32_t cursorCoords = getCoords(editor, cellCursor);
				uint32_t cursorX = (cursorCoords & 0xFFFF0000) >> 16;
				uint32_t cursorY = cursorCoords & 0x0000FFFF;

				if (kDown) {
					if (kDown & KEY_DRIGHT && cursorX < (TOP_SCREEN_WIDTH - editor->cellSize)) cellCursor += 1;
					if (kDown & KEY_DLEFT && cursorX > 0) cellCursor -= 1;
					if (kDown & KEY_DDOWN && cursorY < (TOP_SCREEN_HEIGHT - editor->cellSize)) {
						uint32_t newCoords = (cursorX << 16) | (int)(cursorY + editor->cellSize);
						cellCursor = getIndex(editor, newCoords);
					}
					if (kDown & KEY_DUP && cursorY > 0) {
						uint32_t newCoords = (cursorX << 16) | (int)(cursorY - editor->cellSize);
						cellCursor = getIndex(editor, newCoords);
					}

					//If select button is pressed, go back to main menu
					if (kDown & KEY_SELECT) {
						//When switching to the main menu free the grid from memory and reinitialise the title
						destroyGrid(editor);
						grid = editor;
						titlescreen = newEmptyGrid(4.0f);
						//Set game state and menu selection
						menuSelection = 0;
						gameState = 0;
						break;
					}

					//If A button pressed, place cell
					if (kDown & KEY_A) newCell(editor, cellCursor);

					//If B button pressed, remove cell
					if (kDown & KEY_B) killCell(editor, cellCursor);
				}

				beginFrame();
				C2D_SceneBegin(top);
				//Clear top screen and draw grid
				clrScreen(top, clrBlack);
				draw(editor, top, clrBlack, clrWhite);
				//Draw cursor
				C2D_DrawRectSolid(cursorX,cursorY,0.0f,editor->cellSize,editor->cellSize,clrRed);
				//Clear bottom screen
				C2D_SceneBegin(bottom);
				clrScreen(bottom, clrBlack);
				endFrame();
				break;
		}

		//Reset counter if key is pressed or touch screen is touched
		if (gameState == 0 && (kDown || (touch.px || touch.py))) {
			framesSinceKeyPress = 0;
		}
	}

	//If any pointers are still initialised, free them
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	if (titlescreen != NULL) destroyGrid(titlescreen);
	if (grid != NULL) destroyGrid(grid);
	if (editor != NULL) destroyGrid(editor);
#pragma GCC diagnostic pop

	//Destroy text buffers
	mainMenuDeInit();

	//Deinit libs
	C2D_Fini();
	C3D_Fini();
	romfsExit();
	gfxExit();

	return 0;
}

void beginFrame() {
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
}

void endFrame() {
	C3D_FrameEnd(0);
}

void clrScreen(C3D_RenderTarget *screen, u32 colour) {
	C2D_TargetClear(screen, colour);
}

void mainMenuInit() {
	//Create text buffer
	menuBuffer = C2D_TextBufNew(18);

	//Parse the strings
	C2D_TextParse(&menuText[0], menuBuffer, "START");
	C2D_TextParse(&menuText[1], menuBuffer, "EDITOR");
	C2D_TextParse(&menuText[2], menuBuffer, "EXIT");

	//Optimise text
	C2D_TextOptimize(&menuText[0]);
	C2D_TextOptimize(&menuText[1]);
	C2D_TextOptimize(&menuText[2]);
}

void mainMenuDeInit() {
	//Delete the text buffer
	C2D_TextBufDelete(menuBuffer);
}

void getTitle(Grid *grid) {
	FILE *fp;
	fp = fopen("romfs:/data/title.dat", "r");
	fread(grid->cells, sizeof(char), grid->size, fp);
	fclose(fp);
}

void drawMenu(C3D_RenderTarget *screen, char selection, u32 bgColour, u32 fgColour) {
	//Setup the screen
	clrScreen(screen, bgColour);
	C2D_SceneBegin(screen);

	//Floats for text dimensions and coordsC2D_Scene
	float textWidth;
	float textHeight;
	float x;
	float y;
	float selectionBoxX;
	float selectionBoxY;
	float selectionBoxWidth;
	float selectionBoxHeight;

	for (unsigned char i = 0; i < 3; i++) {
		//Get text dimensions
		C2D_TextGetDimensions(&menuText[i], 0.5f, 0.5f, &textWidth, &textHeight);
		//Calculate vertical spacing
		x = (BOTTOM_SCREEN_WIDTH / 2) - (textWidth / 2);
		y = (((BOTTOM_SCREEN_HEIGHT - 150) * 3 * i) / 6) + 75 - (textHeight / 2);

		if (selection == i) {

			selectionBoxX = x - 6;
			selectionBoxY = y - 2;
			selectionBoxWidth = textWidth + 12;
			selectionBoxHeight = textHeight + 4;

			//Draw Square
			C2D_DrawRectSolid(selectionBoxX, selectionBoxY, 0.0f, selectionBoxWidth, selectionBoxHeight, fgColour);

			//Draw Text
			C2D_DrawText(&menuText[i], C2D_WithColor, x, y, 0.0f, 0.5f, 0.5f, bgColour);

			//Skip to next iteration
			continue;
		}

		//Draw Text
		C2D_DrawText(&menuText[i], C2D_WithColor, x, y, 0.0f, 0.5f, 0.5f, fgColour);
	}
}

void draw(Grid *grid, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour) {
	//Render the scene
	clrScreen(screen, bgColour);
	C2D_SceneBegin(screen);

	//Draw Squares
	drawGrid(grid, fgColour);
}

void updateGrid(Grid *grid) {
	//Update Neighbours for every cell
	for (uint16_t i = 0; i < grid->size; i++) {
		checkNeighbours(grid, i);
	}
	//Update cell states
	for (uint16_t i = 0; i < grid->size; i++) {
		updateCell(grid, i);
	}
}