#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <3ds.h>
#include <citro2d.h>

#include "main.h"
#include "grid.h"

void getTitle(Grid *grid);
void draw(Grid *grid, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour);
void updateGrid(Grid *grid);
void drawMenu(C3D_RenderTarget *screen, char selection, u32 bgColour, u32 fgColour, double frameNum);
void beginFrame();
void endFrame();
void clrScreen(C3D_RenderTarget *screen, u32 colour);
void mainMenuInit();
void mainMenuDeInit();
void gameTextInit();
void gameTextDeInit();

//Create text objects for menu
C2D_TextBuf menuBuffer;
C2D_Text menuText[3];
//Selection box time period for menu
uint16_t selectionTimePeriod = 150;

//Create text objects for game
C2D_TextBuf gameBuffer;
C2D_Text gameText[1];
float pausetextWidth, pausetextHeight, pausetextPadding, pausetextX, pausetextY;

int main(int argc, char const *argv[])
{

	//Init libs
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS * 2);
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
	u32 clrGrey = C2D_Color32f(0.08f,0.08f,0.08f,1.0f);
	u32 clrLightGrey = C2D_Color32f(0.75f,0.75f,0.75f,1.0f);
	u32 clrRed = C2D_Color32f(1.0f,0.0f,0.0f,1.0f);
	u32 clrPink = C2D_Color32f(1.0f,0.42f,0.42f,1.0f);

	//Set game state
	signed char gameState = 0; //0=menu 1=game 2=editor

	//Set menu selection
	signed char menuSelection = 0; //0=START 1=EDITOR 2=EXIT
	double menuFrameNum = 0;

	//Set game paused status
	unsigned char gamePaused = 0;

	//Set cell size
	float cellSize = 4.0f;

	//Generate strings for main menu and game
	mainMenuInit();
	gameTextInit();

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

	//Create empty grid for editor
	Grid *editor = newEmptyGrid(cellSize);
	uint32_t cellCursor = 0;
	unsigned char cursorScale = 1;
	uint32_t leftCursorFrames,rightCursorFrames,upCursorFrames,downCursorFrames;
	unsigned char abDelay = 1;
	unsigned char abFrames = 0;
	leftCursorFrames=rightCursorFrames=upCursorFrames=downCursorFrames = 0;

	//Main loop
	while (aptMainLoop()) {

		//If quit = 1 then quit the game loop
		if (quit) break;

		//Read button and touchscreen inputs
		hidScanInput();
		hidTouchRead(&touch);

		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();

		switch(gameState) {
			case 0:

				if (framesSinceKeyPress > 1200) {
					updateGrid(titlescreen);
					framesSinceKeyPress = 1200; //Keep framesincekeypress at 1200 so as to prevent overflows if the game is left too long
				} else if (framesSinceKeyPress == 0) {
					getTitle(titlescreen);
				}

				if (menuFrameNum == selectionTimePeriod * 10) menuFrameNum = 0;

				//Menu Navigation
				if (kDown & KEY_DDOWN) {
					menuSelection += 1;
					menuFrameNum = 0;
					if (menuSelection > 2) menuSelection = 0;
				}
				if (kDown & KEY_DUP) {
					menuFrameNum = 0;
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
							grid = newEmptyGrid(cellSize);
							fillGridRandom(grid);
							framesSinceKeyPress = 0;
							menuFrameNum = 0;
							gameState = 1;
							break;
						case 1:
							//When switching to editor, free the main menu from memory and initialise the grid
							destroyGrid(titlescreen);
							titlescreen = NULL;
							framesSinceKeyPress = 0;
							menuFrameNum = 0;
							gameState = 2;
							break;
						case 2:
							quit = 1;
							break;
					}
				}

				//Check for exit signal
				if (gameState != 0) {
					break;
				}

				beginFrame();
				//Draw Title Screen
				draw(titlescreen, top, clrGrey, clrLightGrey);
				//Draw Main Menu
				drawMenu(bottom, menuSelection, clrBlack, clrWhite, menuFrameNum);
				endFrame();
				framesSinceKeyPress += 1;
				menuFrameNum += 1;
				break;
			case 1:
				//Randomise grid when A button is pressed
				if (kDown & KEY_A) {
					fillGridRandom(grid);
					beginFrame();
					draw(grid, top, clrGrey, clrLightGrey);
					//Clear bottom screen
					C2D_SceneBegin(bottom);
					clrScreen(bottom, clrBlack);
					endFrame();
					break;
				}

				//If B button is pressed, return to editor
				if ((kDown & KEY_B) && !gamePaused) {
					destroyGrid(grid);
					grid = NULL;
					gamePaused = 0;
					gameState = 2;
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
					draw(grid, top, clrGrey, clrLightGrey);
					//Clear bottom screen
					C2D_SceneBegin(bottom);
					clrScreen(bottom, clrBlack);
					endFrame();
				} else {
					beginFrame();
					if (kDown & KEY_B) {
						updateGrid(grid);
					}
					draw(grid, top, clrGrey, clrLightGrey);
					C2D_DrawText(&gameText[0],C2D_WithColor, pausetextX, pausetextY, 0.0f, 1.0f, 1.0f, clrRed);
					//Clear bottom screen
					C2D_SceneBegin(bottom);
					clrScreen(bottom, clrBlack);
					endFrame();
				}
				break;
			case 2:

				uint32_t cursorCoords = getCoords(editor, cellCursor);
				uint32_t cursorX = (cursorCoords & 0xFFFF0000) >> 16;
				uint32_t cursorY = cursorCoords & 0x0000FFFF;
				uint32_t x,y,cursorIndex;
				char goLeft,goRight,goUp,goDown;
				unsigned char cursorDelay = 20;
				goLeft=goRight=goUp=goDown = 0;

				goLeft = ((kDown & KEY_DLEFT) || ((leftCursorFrames > cursorDelay) && (leftCursorFrames % 2 == 0)) || (leftCursorFrames > cursorDelay * 4)) ? 1 : 0;
				goRight = ((kDown & KEY_DRIGHT) || ((rightCursorFrames > cursorDelay) && (rightCursorFrames % 2 == 0)) || (rightCursorFrames > cursorDelay * 4)) ? 1 : 0;
				goUp = ((kDown & KEY_DUP) || ((upCursorFrames > cursorDelay) && (upCursorFrames % 2 == 0)) || (upCursorFrames > cursorDelay * 4)) ? 1 : 0;
				goDown = ((kDown & KEY_DDOWN) || ((downCursorFrames > cursorDelay) && (downCursorFrames % 2 == 0)) || (downCursorFrames > cursorDelay * 4)) ? 1 : 0;

				//Move cursor
				if (goLeft && cursorX > 0) cellCursor -= 1;
				if (goRight && cursorX < (TOP_SCREEN_WIDTH - (editor->cellSize * cursorScale))) cellCursor += 1;
				if (goUp && cursorY > 0) {
					uint32_t newCoords = (cursorX << 16) | (int)(cursorY - editor->cellSize);
					cellCursor = getIndex(editor, newCoords);
				}
				if (goDown && cursorY < (TOP_SCREEN_HEIGHT - (editor->cellSize * cursorScale))) {
					uint32_t newCoords = (cursorX << 16) | (int)(cursorY + editor->cellSize);
					cellCursor = getIndex(editor, newCoords);
				}

				//Place or remove cells
				if ((kDown || kHeld) && !abDelay) {
					for (int i = 0; i < cursorScale; i++) {
						for (int j = 0; j < cursorScale; j++) {
							cursorIndex = (uint32_t)(cellCursor + i + ((TOP_SCREEN_WIDTH / editor->cellSize) * j));

							//Kill or Place cells
							if ((kDown & KEY_A) || (kHeld & KEY_A)) newCell(editor, cursorIndex);
							else if ((kDown & KEY_B) || (kHeld & KEY_B)) killCell(editor, cursorIndex);
						}
					}
				}

				if (kDown) {

					//If select button is pressed, go back to main menu
					if (kDown & KEY_SELECT) {
						//When switching to the main menu free the grid from memory and reinitialise the title
						titlescreen = newEmptyGrid(4.0f);
						//Set game state and menu selection
						menuSelection = 0;
						gameState = 0;
						abDelay = 1;
						break;
					}

					//If Y is Pressed, clear the screen
					if (kDown & KEY_Y) {
						for (int i = 0; i < editor->size; i++) {
							editor->cells[i] = (char)0;
						}
					}

					//If L button pressed, shrink cursor
					if ((kDown & KEY_L) && (cursorScale > 1)) cursorScale -= 1;

					//If R button pressed, enlarge the cursor
					if ((kDown & KEY_R) && (cursorScale < 20) && cursorX < (TOP_SCREEN_WIDTH - (editor->cellSize * cursorScale)) && cursorY < (TOP_SCREEN_HEIGHT - (editor->cellSize * cursorScale))) {
						cursorScale += 1;
					}

					//If START button pressed, start game
					if (kDown & KEY_START) {
						grid = newEmptyGrid(editor->cellSize);
						if (grid == NULL) break;

						for (int i = 0;i < grid->size;i++) {
							grid->cells[i] = editor->cells[i];
						}

						framesSinceKeyPress = 0;
						//Pause the game by default;
						gamePaused = 1;
						abDelay = 1;
						gameState = 1;
						break;
					}
				}

				//Count length of held down keys
				if (kHeld & KEY_DLEFT) leftCursorFrames += 1;
				if (kHeld & KEY_DRIGHT) rightCursorFrames += 1;
				if (kHeld & KEY_DUP) upCursorFrames += 1;
				if (kHeld & KEY_DDOWN) downCursorFrames += 1;

				//Reset counters if keys are released
				if (kUp & KEY_DLEFT) leftCursorFrames = 0;
				if (kUp & KEY_DRIGHT) rightCursorFrames = 0;
				if (kUp & KEY_DUP) upCursorFrames = 0;
				if (kUp & KEY_DDOWN) downCursorFrames = 0;

				//Increment abframes
				if (abDelay) abFrames += 1;
				if (abFrames > 50) abDelay = abFrames = 0;

				beginFrame();
				C2D_SceneBegin(top);
				
				//Clear top screen and draw grid
				draw(editor, top, clrGrey, clrLightGrey);
				
				//Draw cursor

				for (int i = 0; i < cursorScale; i++) {
					for (int j = 0; j < cursorScale; j++) {
						x = cursorX + (i * editor->cellSize);
						y = cursorY + (j * editor->cellSize);

						cursorIndex = (uint32_t)(cellCursor + i + ((TOP_SCREEN_WIDTH / editor->cellSize) * j));

						if (isAlive(editor->cells[cursorIndex])) {
							C2D_DrawRectSolid(x,y,0.0f,editor->cellSize,editor->cellSize,clrPink);
						} else {
							C2D_DrawRectSolid(x,y,0.0f,editor->cellSize,editor->cellSize,clrRed);
						}
					}
				}

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
	gameTextDeInit();

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

void gameTextInit() {
	gameBuffer = C2D_TextBufNew(7);
	C2D_TextParse(&gameText[0], gameBuffer, "PAUSED");
	C2D_TextOptimize(&gameText[0]);
	pausetextPadding = 5.0f;
	C2D_TextGetDimensions(&gameText[0], 1.0f, 1.0f, &pausetextWidth, &pausetextHeight);
	pausetextX = TOP_SCREEN_WIDTH - pausetextWidth - pausetextPadding;
	pausetextY = pausetextPadding;
}

void gameTextDeInit() {
	//Delete the text buffer
	C2D_TextBufDelete(gameBuffer);
}

void getTitle(Grid *grid) {
	FILE *fp;
	fp = fopen("romfs:/data/title.dat", "r");
	fread(grid->cells, sizeof(char), grid->size, fp);
	fclose(fp);
}

void drawMenu(C3D_RenderTarget *screen, char selection, u32 bgColour, u32 fgColour, double frameNum) {
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

	u32 selectionColour;
	u32 textColour;

	//Get selection box colour
	if (frameNum > 20) {
		float colourVal1 = (float) pow(cos(frameNum * (M_PI/selectionTimePeriod)),2);
		float colourVal2 = (float) pow(sin(frameNum * (M_PI/selectionTimePeriod)),2);
		selectionColour = C2D_Color32f(colourVal1,colourVal1,colourVal1,1.0f);
		textColour = C2D_Color32f(colourVal2,colourVal2,colourVal2,1.0f);
	} else {
		selectionColour = fgColour;
		textColour = bgColour;
	}

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
			C2D_DrawRectSolid(selectionBoxX, selectionBoxY, 0.0f, selectionBoxWidth, selectionBoxHeight, selectionColour);

			//Draw Text
			C2D_DrawText(&menuText[i], C2D_WithColor, x, y, 0.0f, 0.5f, 0.5f, textColour);

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