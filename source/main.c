#include <stdlib.h>
#include <stdio.h>

#include "grid.h"

#include <3ds.h>
#include <citro2d.h>

void getTitle(Grid *grid);
void draw(Grid *grid, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour);
void updateGrid(Grid *grid);

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

	//Define Colours
	u32 clrWhite = C2D_Color32f(1.0f,1.0f,1.0f,1.0f);
	u32 clrBlack = C2D_Color32f(0.0f,0.0f,0.0f,0.0f);

	//Set game state
	char gameState = 0; //0=menu 1=game 2=editor

	//Create counter for frames since a key was pressed
	uint32_t framesSinceKeyPress = 0;

	//Create touch position object for touchscreen pressed
	touchPosition touch;

	//Create Empty Grid for titlescreen
	Grid *titlescreen = newEmptyGrid(4.0f);

	//Read title screen from romfs
	getTitle(titlescreen);

	//Create empty grid for game
	Grid *grid = newEmptyGrid(4.0f);
	//Randomise grid
	fillGridRandom(grid);


	//Main loop
	while (aptMainLoop()) {

		//Read button and touchscreen inputs
		hidScanInput();
		hidTouchRead(&touch);

		u32 kDown = hidKeysDown();

		//Check for start
		if (kDown & KEY_START) break;

		switch(gameState) {
			case 0:
				if (framesSinceKeyPress > 1200) {
					updateGrid(titlescreen);
				} else if (framesSinceKeyPress == 0) {
					getTitle(titlescreen);
				}

				//If select button is pressed, start game
				if (kDown & KEY_SELECT) {
					framesSinceKeyPress = 0;
					gameState = 1;
				}

				draw(titlescreen, top, clrBlack, clrWhite);
				framesSinceKeyPress += 1;
				break;
			case 1:
				//Randomise grid when a button is pressed
				if (kDown & KEY_A) {
					fillGridRandom(grid);
				}

				//If select button is pressed, go back to main menu
				if (kDown & KEY_SELECT) {
					gameState = 0;
				}

				//Update cells then draw
				updateGrid(grid);
				draw(grid, top, clrBlack, clrWhite);
				break;
		}

		//Reset counter if key is pressed or touch screen is touched
		if (gameState == 0 && (kDown || (touch.px || touch.py))) {
			framesSinceKeyPress = 0;
		}
	}

	//Free grid memory
	destroyGrid(grid);

	//Deinit libs
	C2D_Fini();
	C3D_Fini();
	romfsExit();
	gfxExit();

	return 0;
}

void getTitle(Grid *grid) {
	FILE *fp;
	fp = fopen("romfs:/data/title.dat", "r");
	fread(grid->cells, sizeof(char), grid->size, fp);
	fclose(fp);
}

void draw(Grid *grid, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour) {
	//Render the scene
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(screen, bgColour);
	C2D_SceneBegin(screen);

	//Draw Squares
	drawGrid(grid, fgColour);

	C3D_FrameEnd(0);
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