#ifndef _COMMON_H_GUARD_
#define _COMMON_H_GUARD_

#define TOP_SCREEN_WIDTH 400
#define TOP_SCREEN_HEIGHT 240
#define BOTTOM_SCREEN_WIDTH 320
#define BOTTOM_SCREEN_HEIGHT 240

//                             aabbggrr - aa=alpha bb=blue gg=green rr=red
#define COLOR_WHITE     (u32)0xffffffff 
#define COLOR_BLACK     (u32)0xff000000
#define COLOR_GREY      (u32)0xff141414
#define COLOR_LIGHTGREY (u32)0xffc0c0c0
#define COLOR_RED       (u32)0xff0000ff
#define COLOR_PINK      (u32)0xff6b6bff

#include "grid.h"

extern void beginFrame();
extern void endFrame();
extern void clrScreen(C3D_RenderTarget *screen, u32 colour);

typedef enum {
    TOP_SCREEN,
    BOTTOM_SCREEN,
} ScreenType;

/**
 * @brief Draws a grid to the screen.
 * 
 * @param grid A pointer to the Grid to render
 * @param screen A pointer to the screen on which to render the grid
 * @param bgColour The background colour of the grid
 * @param fgColour The foreground colour of the grid
 */
extern void draw(Grid *grid, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour);

#endif