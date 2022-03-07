#include "menu.h"

#include <stdlib.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

Menu *initMenu() {
    Menu *m = malloc(sizeof(Menu));
    if (m == NULL)
        return NULL;

    m->menuBuffer = C2D_TextBufNew(18);
    if (m->menuBuffer == NULL)
        goto error;

    //Parse the strings
	if (
        C2D_TextParse(m->menuText + 0, m->menuBuffer, "START") == NULL ||
	    C2D_TextParse(m->menuText + 1, m->menuBuffer, "EDITOR") == NULL ||
	    C2D_TextParse(m->menuText + 2, m->menuBuffer, "EXIT") == NULL
    )
        goto error;

    m->menuFrameNum = (double)0;
    m->selectionTimePeriod = (uint16_t)150;

    // Create Empty Grid for titlescreen
    m->titleScreen = newEmptyGrid(4.0f);
    if (m->titleScreen == NULL)
        goto error;

	//Optimise text
    C2D_TextOptimize(m->menuText + 0);
	C2D_TextOptimize(m->menuText + 1);
	C2D_TextOptimize(m->menuText + 2);

    return m;

error:
    destroyMenu(m);
    return NULL;
}

void destroyMenu(Menu *menu) {
    if (menu == NULL)
        return;
    
    //Delete the text buffer
    C2D_TextBufDelete(menu->menuBuffer);

    // Destroy titleScreen grid
    destroyGrid(menu->titleScreen);

    free(menu);
}

int doMenuFrame(Menu *m, GlobalState *s) {
    if (s->framesSinceKeyPress > 1200) {
        updateGrid(m->titleScreen);
		s->framesSinceKeyPress = 1200; //Keep framesincekeypress at 1200 so as to prevent overflows if the game is left too long
    } else if (s->framesSinceKeyPress == 0) {
        if (getMenuTitle(m->titleScreen)) 
            // error
            return 1;
    }

    //Menu Navigation
    if (keyDown(s, KEY_DDOWN)) {
        m->selectedOption += 1;
        if (m->selectedOption > 2) 
            m->selectedOption = 0;
    }
    if (keyDown(s, KEY_DUP)) {
        m->selectedOption -= 1;
        if (m->selectedOption < 0) 
            m->selectedOption = 2;
    }

    //Menu Logic
    if (keyHeld(s, KEY_A)) {
        switch(m->selectedOption) {
            case 0:
                /*s->grid = newEmptyGrid(4.0f);
                fillGridRandom(s->grid);*/
                s->framesSinceKeyPress = 0;
                // all we have to do is set the new gamestate and the gameloop will detect this
                // and initialise the new state, as well as deinitialise the current state
                s->gameState = GS_GAME;
                return 0;

                /*//Create game text buffers
                gameBuffer = C2D_TextBufNew(7);
                C2D_TextParse(&gameText[0], gameBuffer, "PAUSED");
                C2D_TextOptimize(&gameText[0]);
                pausetextPadding = 5.0f;
                C2D_TextGetDimensions(&gameText[0], 1.0f, 1.0f, &pausetextWidth, &pausetextHeight);
                pausetextX = TOP_SCREEN_WIDTH - pausetextWidth - pausetextPadding;
                pausetextY = pausetextPadding;
                break;*/
            case 1:
                s->framesSinceKeyPress = 0;
                // all we have to do is set the new gamestate and the gameloop will detect this
                // and initialise the new state, as well as deinitialise the current state
                s->gameState = GS_EDITOR;
                return 0;
            
            case 2:
                s->quit = 1;
                break;
        }
    }

    beginFrame();
    
    //Draw Title Screen
    draw(m->titleScreen, s->top, COLOR_BLACK, COLOR_WHITE);

    //Draw Main Menu
    drawMenu(m, s->bottom, COLOR_BLACK, COLOR_WHITE, m->menuFrameNum);
	
    endFrame();
    
    s->framesSinceKeyPress += 1;
    m->menuFrameNum += 1;
    
    return 0;
}

int getMenuTitle(Grid *grid) {
	FILE *fp = fopen("romfs:/data/title.dat", "r");
    if (fp == NULL)
        return 1;
        
    size_t bytes_read = fread(grid->cells, sizeof(char), grid->size, fp);
	fclose(fp);

    if (bytes_read != grid->size)
        return 1;

    return 0;
}

void drawMenu(Menu *m, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour, double menuFrame) {
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
	if (menuFrame > 20) {
		float colourVal1 = (float)pow(cos(menuFrame * (M_PI/m->selectionTimePeriod)),2);
		float colourVal2 = (float)pow(sin(menuFrame * (M_PI/m->selectionTimePeriod)),2);
		selectionColour = C2D_Color32f(colourVal1,colourVal1,colourVal1,1.0f);
		textColour = C2D_Color32f(colourVal2,colourVal2,colourVal2,1.0f);
	} else {
		selectionColour = fgColour;
		textColour = bgColour;
	}

	for (unsigned char i = 0; i < 3; i++) {
		//Get text dimensions
		C2D_TextGetDimensions(m->menuText + i, 0.5f, 0.5f, &textWidth, &textHeight);
		
        //Calculate vertical spacing
		x = (BOTTOM_SCREEN_WIDTH / 2) - (textWidth / 2);
		y = (((BOTTOM_SCREEN_HEIGHT - 150) * 3 * i) / 6) + 75 - (textHeight / 2);

		if (m->selectedOption == i) {

			selectionBoxX = x - 6;
			selectionBoxY = y - 2;
			selectionBoxWidth = textWidth + 12;
			selectionBoxHeight = textHeight + 4;

			//Draw Square
			C2D_DrawRectSolid(selectionBoxX, selectionBoxY, 0.0f, selectionBoxWidth, selectionBoxHeight, selectionColour);

			//Draw Text
			C2D_DrawText(m->menuText + i, C2D_WithColor, x, y, 0.0f, 0.5f, 0.5f, textColour);

			//Skip to next iteration
			continue;
		}

		//Draw Text
		C2D_DrawText(m->menuText + i, C2D_WithColor, x, y, 0.0f, 0.5f, 0.5f, fgColour);
	}
}