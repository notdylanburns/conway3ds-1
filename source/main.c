#include <stdlib.h>
#include <stdio.h>

#include "grid.h"

#include <3ds.h>
#include <citro2d.h>

int main(int argc, char const *argv[])
{
	//Init libs
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	//Create screens
	C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	//Define Colours
	u32 clrWhite = C2D_Color32f(1.0f,1.0f,1.0f,1.0f);
	u32 clrBlack = C2D_Color32f(0.0f,0.0f,0.0f,0.0f);

	//Create Empty Grid
	Grid *grid = newEmptyGrid(4.0f);
	//Randomise grid
	fillGridRandom(grid);

	//Main loop
	while (aptMainLoop()) {
		hidScanInput();

		u32 kDown = hidKeysDown();

		//Check for start
		if (kDown & KEY_START) break;
		//Check for a button
		if (kDown & KEY_A) {
			fillGridRandom(grid);
		}


		//Update Neighbours for every cell
		for (uint16_t i = 0; i < grid->size; i++) {
			checkNeighbours(grid, i);
		}
		//Update cell states
		for (uint16_t i = 0; i < grid->size; i++) {
			updateCell(grid, i);
		}


		//Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, clrBlack);
		C2D_SceneBegin(top);

		//Draw Squares
		drawGrid(grid, clrWhite);

		C3D_FrameEnd(0);
	}

	//Free grid memory
	destroyGrid(grid);

	//Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();

	return 0;
}