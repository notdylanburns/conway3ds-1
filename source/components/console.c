#include "console.h"

#include "../common.h"

#include <stdlib.h>

#include <3ds/font.h>

Console *initConsole(Console *console, ScreenType screen) {
    Console *c = console;
    if (c == NULL) {
        c = malloc(sizeof(Console));
        if (c == NULL)
            return NULL;

        FINF_s *font = C2D_FontGetInfo(NULL);
        int screenWidth = (screen == TOP_SCREEN) ? TOP_SCREEN_WIDTH : BOTTOM_SCREEN_WIDTH;
        int screenHeight = (screen == TOP_SCREEN) ? TOP_SCREEN_HEIGHT : BOTTOM_SCREEN_HEIGHT;

        c->width = screenWidth / font->width;
        c->height = screenHeight / font->height;
    }

    // create a text buffer the size of the screen
    c->textBuffer = C2D_TextBufNew(c->width * c->height);

    return c;
}