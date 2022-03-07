#include "state.h"

void updateGlobalState(GlobalState *g) {
    hidScanInput();
	hidTouchRead(&g->touch);

	g->kDown = hidKeysDown();
    g->kUp = hidKeysUp();
    g->kHeld = hidKeysHeld();
}

