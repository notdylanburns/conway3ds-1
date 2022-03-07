#ifndef _MENU_H_GUARD_
#define _MENU_H_GUARD_

#include <3ds.h>
#include <citro2d.h>

#include "../common.h"
#include "../grid.h"
#include "../state.h"

typedef struct {
    C2D_TextBuf menuBuffer;
    C2D_Text menuText[3];

    char selectedOption;

    Grid *titleScreen;
    double menuFrameNum;
    uint16_t selectionTimePeriod;
} Menu;

/**
 * @brief Initialises a Menu and its corresponding C2D_TextBufs. Returns NULL on failure.
 * 
 * @return Menu * 
 */
extern Menu *initMenu();

/**
 * @brief Deallocates the memory for the supplied Menu.
 * 
 * @param menu A pointer to the Menu to destroy
 */
extern void destroyMenu(Menu *menu);

/**
 * @brief Causes the Menu state to update. Returns 1 on failure.
 * 
 * @param m A pointer to the Menu to update
 * @param s A pointer to a GlobalState, holding the current state of the program
 * @return int 
 */
extern int doMenuFrame(Menu *m, GlobalState *s);

/**
 * @brief Retrieves the menu title from the romfs. Returns 1 on failure.
 * 
 * @param grid A pointer to the Grid to populate with the tile data
 * @return int 
 */
extern int getMenuTitle(Grid *grid);

/**
 * @brief Draws the Menu and highlights the selected option.
 * 
 * @param m A pointer to the Menu to render
 * @param screen A pointer to the screen on which to render the Menu
 * @param bgColour The background colour of the menu
 * @param fgColour The foreground colour of the menu
 * @param menuFrame The current frame of the menu
 */
extern void drawMenu(Menu *m, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour, double menuFrame);

#endif