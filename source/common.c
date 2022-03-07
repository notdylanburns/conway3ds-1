#include "common.h"

#include <citro2d.h>
#include <citro3d.h>

void beginFrame() {
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
}

void endFrame() {
	C3D_FrameEnd(0);
}

void clrScreen(C3D_RenderTarget *screen, u32 colour) {
	C2D_TargetClear(screen, colour);
}

void draw(Grid *grid, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour) {
	//Render the scene
	clrScreen(screen, bgColour);
	C2D_SceneBegin(screen);

	//Draw Squares
	drawGrid(grid, fgColour);
}