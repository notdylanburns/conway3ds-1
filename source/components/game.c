#include "game.h"

#include <stdlib.h>

#include "../common.h"

Game *initGame(Game *game, float cellSize) {
    Game *g = game;
    if (g == NULL) {
        g = malloc(sizeof(Game));
        if (g == NULL)
            // error
            return NULL;

        g->grid = newEmptyGrid(cellSize);
        if (g->grid == NULL) {
            free(g);
            return NULL;
        }

        fillGridRandom(g->grid);

        g->paused = false;
    } else {

        g->gameBuffer = C2D_TextBufNew(7);
        if (g->gameBuffer == NULL) {
            destroyGame(g);
            return NULL;
        }

        if (C2D_TextParse(g->gameText + 0, g->gameBuffer, "PAUSED") == NULL) {
            destroyGame(g);
            return NULL;
        }

        C2D_TextOptimize(g->gameText + 0);
        g->pauseTextPadding = 5.0f;
        C2D_TextGetDimensions(g->gameText + 0, 1.0f, 1.0f, &g->pauseTextWidth, &g->pauseTextHeight);
        g->pauseTextX = TOP_SCREEN_WIDTH - g->pauseTextWidth - g->pauseTextPadding;
        g->pauseTextY = g->pauseTextPadding;
    }

    return g;
}

void destroyGame(Game *game) {
    C2D_TextBufDelete(game->gameBuffer);
}

void destroyFullyGame(Game *game) {
    if (game == NULL)
        return;

    C2D_TextBufDelete(game->gameBuffer);
    destroyGrid(game->grid);
    free(game);
}

void setGameGrid(Game *game, Grid *newGrid) {
    memcpy(game->grid->cells, newGrid->cells, newGrid->size);
}

int doGameFrame(Game *g, GlobalState *s) {
    if (keyDown(s, KEY_A)) {
        fillGridRandom(g->grid);
        beginFrame();
        
        draw(g->grid, s->top, COLOR_GREY, COLOR_LIGHTGREY);
        //Clear bottom screen
        C2D_SceneBegin(s->bottom);
        clrScreen(s->bottom, COLOR_BLACK);
        endFrame();
        return 0;
    }

    //If B button is pressed, return to editor
    if (keyDown(s, KEY_B) && !g->paused) {
        g->paused = 0;
        // all we have to do is set the new gamestate and the gameloop will detect this
        // and initialise the new state, as well as deinitialise the current state
        s->gameState = GS_EDITOR;
        return 0;
    }

    //If start button is pressed, pause game
    if (keyDown(s, KEY_START)) {
        g->paused = !g->paused;
    }

    //If select button is pressed, go back to main menu
    if (keyDown(s, KEY_SELECT)) {
        //Set game state and menu selection and unpause game
        g->paused = 0;
        
        // all we have to do is set the new gamestate and the gameloop will detect this
        // and initialise the new state, as well as deinitialise the current state
        s->gameState = GS_MENU;
        return 0;
    }

    drawGame(g, s, s->top, COLOR_GREY, COLOR_LIGHTGREY);

    return 0;
}

void drawGame(Game *g, GlobalState *s, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour) {
    //Update screen if the game isn't paused
    if (!g->paused) {
        //Update cells then draw
        updateGrid(g->grid);
        
        beginFrame();
        
        draw(g->grid, screen, bgColour, fgColour);
    } else {
        beginFrame();

        draw(g->grid, screen, bgColour, fgColour);
        C2D_DrawText(g->gameText + 0,C2D_WithColor, g->pauseTextX, g->pauseTextY, 0.0f, 1.0f, 1.0f, COLOR_RED);
    }

    if (!s->preserveBottom) {
        //Clear bottom screen
        C2D_SceneBegin(s->bottom);
        clrScreen(s->bottom, COLOR_BLACK);
    }

    endFrame();
}