#include "state.h"

void updateGlobalState(GlobalState *g) {
    hidScanInput();
	hidTouchRead(&g->touch);

	g->kDown = hidKeysDown();
    g->kUp = hidKeysUp();
    g->kHeld = hidKeysHeld();
}

unsigned char keyHeld(GlobalState *s, int key) {
    return (s->kHeld & key) > 0;
}

unsigned char keyDown(GlobalState *s, int key) {
    return (s->kDown & key) > 0;
}

unsigned char keyUp(GlobalState *s, int key) {
    return (s->kUp & key) > 0;
}