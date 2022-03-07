#ifndef _STATE_H_GUARD_
#define _STATE_H_GUARD_

#include <stdint.h>

#include <3ds.h>
#include <citro3d.h>

#include "grid.h"

enum GameState {
    GS_MENU,
    GS_GAME,
    GS_EDITOR,
    GS_CONSOLE,

    GS_TOTAL,
};

typedef struct {
    unsigned char paused; // boolean, 1 if game is paused, else 0
    unsigned char quit; // boolean, 1 if game should quit, else 0
    unsigned char preserveBottom; // boolean, if 0 the bottom screen will be cleared at the end of the frame, else 1

    uint32_t framesSinceKeyPress; // frames since a key was pressed
    uint32_t kDown, kUp, kHeld;

    float cellSize; // Pixel size of cells

    touchPosition touch; // the position of the last touchscreen touch

    enum GameState gameState; // 0=menu 1=game 2=editor 3=console

    C3D_RenderTarget *top;      // Render target for the top screen
    C3D_RenderTarget *bottom;   // Render target for the bottom screen
} GlobalState;

extern void updateGlobalState(GlobalState *g);

/**
 * @brief Checks if a button is held. Returns 1 if the selected button is held, else 0.
 * 
 * @param s A pointer to a GlobalState containing the current state of the program
 * @param key An int holding the numerical code for the button to be checked
 * @return unsigned char 
 */
inline unsigned char keyHeld(GlobalState *s, int key) {
    return (s->kHeld & key) > 0;
}

/**
 * @brief Checks if a button is down. Returns 1 if the selected button is down, else 0.
 * 
 * @param s A pointer to a GlobalState containing the current state of the program
 * @param key An int holding the numerical code for the button to be checked
 * @return unsigned char 
 */
inline unsigned char keyDown(GlobalState *s, int key) {
    return (s->kDown & key) > 0;
}

/**
 * @brief Checks if a button is up. Returns 1 if the selected button is up, else 0.
 * 
 * @param s A pointer to a GlobalState containing the current state of the program
 * @param key An int holding the numerical code for the button to be checked
 * @return unsigned char 
 */
inline unsigned char keyUp(GlobalState *s, int key) {
    return (s->kUp & key) > 0;
}

#endif