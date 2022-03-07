#ifndef _GAME_H_GUARD_
#define _GAME_H_GUARD_

#include "../state.h"
#include "../grid.h"

typedef struct {
    Grid *grid;
    unsigned char paused;

    //Create text objects for game
    C2D_TextBuf gameBuffer;
    C2D_Text gameText[1];

    float pauseTextWidth, pauseTextHeight, pauseTextPadding, pauseTextX, pauseTextY;
} Game;

/**
 * @brief Initialises a Game, or reinitialises it if game != NULL. Returns NULL on failure.
 * 
 * @param game A pointer to the Game to reinitialise
 * @param cellSize A float representing the pixel size of each cell. Should only be set on first initialisation 
 * @return Game * 
 */
extern Game *initGame(Game *game, float cellSize);

/**
 * @brief Deinitialises a Game to later be reinitialised through initGame().
 * 
 * @param game A pointer to the Game to destroy
 */
extern void destroyGame(Game *game);

/**
 * @brief Fully deinitialises a Game.
 * 
 * @param game A pointer to the Game to destroy
 */
extern void destroyFullyGame(Game *game);

/**
 * @brief Set the Grid property of a Game.
 * 
 * @param game A pointer to the Game
 * @param newGrid A pointer to the new Grid
 */
extern void setGameGrid(Game *game, Grid *newGrid);

/**
 * @brief Causes the Game state to update. Returns 1 on failure.
 * 
 * @param g A pointer to the Game to update
 * @param s A pointer to a GlobalState, holding the current state of the program
 * @return int 
 */
extern int doGameFrame(Game *g, GlobalState *s);

/**
 * @brief Draws the Game.
 * 
 * @param g A pointer to the Game to render
 * @param s A pointer to a GlobalState
 * @param screen A pointer to the screen on which to render the Game
 * @param bgColour The background colour of the game
 * @param fgColour The foreground colour of the game
 */
extern void drawGame(Game *g, GlobalState *s, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour);

#endif