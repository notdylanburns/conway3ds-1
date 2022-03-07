#include "editor.h"

#include <stdlib.h>

#include "../common.h"

Editor *initEditor(Editor *editor, float cellSize) {
    Editor *e = editor;
    if (e == NULL) {
        e = malloc(sizeof(Editor));
        if (e == NULL)
            return NULL; 
    
        e->grid = newEmptyGrid(cellSize);
        if (e->grid == NULL) {
            free(e);
            return NULL;
        }

        e->cellCursor = 0;
        e->cursorScale = 1;
        e->abDelay = 1;
        e->abFrames = 0;

        e->leftCursorFrames = 0;
        e->upCursorFrames = 0;
        e->rightCursorFrames = 0;
        e->downCursorFrames = 0;
    }

    return e;

}

void destroyEditor(Editor *editor) {
    // here for consistency sake
    // this function is called whenever the gameState changes
    // so should not deallocate any memory that needs to exist until the end of the program
    // but can deallocate any text buffers
    return;
}

void destroyFullyEditor(Editor *editor) {
    if (editor == NULL)
        return;

    destroyGrid(editor->grid);
    free(editor);
}

void setEditorGrid(Editor *editor, Grid *newGrid) {
    memcpy(editor->grid->cells, newGrid->cells, newGrid->size);
}

int doEditorFrame(Editor *e, GlobalState *s) {
    uint32_t cursorCoords = getCoords(e->grid, e->cellCursor);
    uint32_t cursorX = (cursorCoords & 0xFFFF0000) >> 16;
    uint32_t cursorY = cursorCoords & 0x0000FFFF;
    uint32_t cursorIndex;
    char goLeft,goRight,goUp,goDown;
    unsigned char cursorDelay = 20;
    goLeft=goRight=goUp=goDown = 0;

    goLeft = ((keyDown(s, KEY_DLEFT)) || ((e->leftCursorFrames > cursorDelay) && (e->leftCursorFrames % 2 == 0)) || (e->leftCursorFrames > cursorDelay * 4)) ? 1 : 0;
    goRight = ((keyDown(s, KEY_DRIGHT)) || ((e->rightCursorFrames > cursorDelay) && (e->rightCursorFrames % 2 == 0)) || (e->rightCursorFrames > cursorDelay * 4)) ? 1 : 0;
    goUp = ((keyDown(s, KEY_DUP)) || ((e->upCursorFrames > cursorDelay) && (e->upCursorFrames % 2 == 0)) || (e->upCursorFrames > cursorDelay * 4)) ? 1 : 0;
    goDown = ((keyDown(s, KEY_DDOWN)) || ((e->downCursorFrames > cursorDelay) && (e->downCursorFrames % 2 == 0)) || (e->downCursorFrames > cursorDelay * 4)) ? 1 : 0;

    //Move cursor
    if (goLeft && cursorX > 0) 
        e->cellCursor -= 1;
    
    if (goRight && cursorX < (TOP_SCREEN_WIDTH - (e->grid->cellSize * e->cursorScale))) 
        e->cellCursor += 1;
    
    if (goUp && cursorY > 0) {
        uint32_t newCoords = (cursorX << 16) | (int)(cursorY - e->grid->cellSize);
        e->cellCursor = getIndex(e->grid, newCoords);
    }
    
    if (goDown && cursorY < (TOP_SCREEN_HEIGHT - (e->grid->cellSize * e->cursorScale))) {
        uint32_t newCoords = (cursorX << 16) | (int)(cursorY + e->grid->cellSize);
        e->cellCursor = getIndex(e->grid, newCoords);
    }

    // Place or remove cells
    if ((s->kDown || s->kHeld) && !e->abDelay) {
        for (int i = 0; i < e->cursorScale; i++) {
            for (int j = 0; j < e->cursorScale; j++) {
                cursorIndex = (uint32_t)(e->cellCursor + i + ((TOP_SCREEN_WIDTH / e->grid->cellSize) * j));

                //Kill or Place cells
                if (keyDown(s, KEY_A) || keyHeld(s, KEY_A)) 
                    newCell(e->grid, cursorIndex);
                else if (keyDown(s, KEY_B) || keyHeld(s, KEY_B)) 
                    killCell(e->grid, cursorIndex);
            }
        }
    }

    if (s->kDown) {

        //If select button is pressed, go back to main menu
        if (keyDown(s, KEY_SELECT)) {
            // all we have to do is set the new gamestate and the gameloop will detect this
            // and initialise the new state, as well as deinitialise the current state
            s->gameState = GS_MENU;
            e->abDelay = 1;
            return 0;
        }

        //If Y is Pressed, clear the screen
        if (keyDown(s, KEY_Y)) {
            memset(e->grid->cells, 0, e->grid->size);
        }

        //If L button pressed, shrink cursor
        if (keyDown(s, KEY_L) && (e->cursorScale > 1)) 
            e->cursorScale -= 1;

        //If R button pressed, enlarge the cursor
        if ((keyDown(s, KEY_R) && (e->cursorScale < 20) && cursorX < (TOP_SCREEN_WIDTH - (e->grid->cellSize * e->cursorScale)) && cursorY < (TOP_SCREEN_HEIGHT - (e->grid->cellSize * e->cursorScale)))) {
            e->cursorScale += 1;
        }

        //If START button pressed, start game
        if (keyDown(s, KEY_START)) {
            s->gameState = GS_GAME;

            s->framesSinceKeyPress = 0;
            
            //Pause the game by default;
            e->abDelay = 1;
            return 0;
        }
    }

    //Count length of held down keys
    if (keyHeld(s, KEY_DLEFT)) 
        e->leftCursorFrames += 1;
    
    if (keyHeld(s, KEY_DRIGHT)) 
        e->rightCursorFrames += 1;
    
    if (keyHeld(s, KEY_DUP)) 
        e->upCursorFrames += 1;
    
    if (keyHeld(s, KEY_DDOWN)) 
        e->downCursorFrames += 1;

    //Reset counters if keys are released
    if (keyUp(s, KEY_DLEFT)) 
        e->leftCursorFrames = 0;
    
    if (keyUp(s, KEY_DRIGHT))
        e->rightCursorFrames = 0;
    
    if (keyUp(s, KEY_DUP)) 
        e->upCursorFrames = 0;
    
    if (keyUp(s, KEY_DDOWN))
        e->downCursorFrames = 0;

    //Increment abframes
    if (e->abDelay) 
        e->abFrames += 1;

    if (e->abFrames > 50) 
        e->abDelay = e->abFrames = 0;

    drawEditor(e, s, s->top, COLOR_GREY, COLOR_LIGHTGREY);

    return 0;
}

void drawEditor(Editor *e, GlobalState *s, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour) {
    uint32_t cursorCoords = getCoords(e->grid, e->cellCursor);
    uint32_t cursorX = (cursorCoords & 0xFFFF0000) >> 16;
    uint32_t cursorY = cursorCoords & 0x0000FFFF;
    uint32_t x,y,cursorIndex;

    beginFrame();
    C2D_SceneBegin(screen);
    
    //Clear top screen and draw grid
    draw(e->grid, screen, bgColour, fgColour);
    
    //Draw cursor

    for (int i = 0; i < e->cursorScale; i++) {
        for (int j = 0; j < e->cursorScale; j++) {
            x = cursorX + (i * e->grid->cellSize);
            y = cursorY + (j * e->grid->cellSize);

            cursorIndex = (uint32_t)(e->cellCursor + i + ((TOP_SCREEN_WIDTH / e->grid->cellSize) * j));

            if (isAlive(e->grid->cells[cursorIndex])) {
                C2D_DrawRectSolid(x,y,0.0f,e->grid->cellSize,e->grid->cellSize,COLOR_PINK);
            } else {
                C2D_DrawRectSolid(x,y,0.0f,e->grid->cellSize,e->grid->cellSize,COLOR_RED);
            }
        }
    }

    if (!s->preserveBottom) {
        //Clear bottom screen
        C2D_SceneBegin(s->bottom);
        clrScreen(s->bottom, COLOR_BLACK);
    }

    endFrame();
}