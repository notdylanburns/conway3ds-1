#ifndef _EDITOR_H_GUARD_
#define _EDITOR_H_GUARD_

#include "../grid.h"
#include "../state.h"

typedef struct {
    Grid *grid;
    uint32_t cellCursor;
	unsigned char cursorScale;
	uint32_t leftCursorFrames, rightCursorFrames, upCursorFrames, downCursorFrames;
	unsigned char abDelay;
	unsigned char abFrames;
} Editor;

/**
 * @brief Initialises a Editor, or reinitialises it if editor != NULL. Returns NULL on failure.
 * 
 * @param editor A pointer to the Editor to reinitialise
 * @param cellSize A float representing the pixel size of the editor grid cells
 * @return Editor * 
 */
extern Editor *initEditor(Editor *editor, float cellSize);

/**
 * @brief Deinitialises an Editor to later be reinitialised through initEditor().
 * 
 * @param editor A pointer to the Editor to destroy
 */
extern void destroyEditor(Editor *editor);

/**
 * @brief Fully deinitialises an Editor.
 * 
 * @param editor A pointer to the Editor to destroy
 */
extern void destroyFullyEditor(Editor *editor);

/**
 * @brief Set the Grid property of an Editor.
 * 
 * @param editor A pointer to the Editor
 * @param newGrid A pointer to the new Grid
 */
extern void setEditorGrid(Editor *editor, Grid *newGrid);

/**
 * @brief Causes the Editor state to update. Returns 1 on failure.
 * 
 * @param e A pointer to the Editor to update
 * @param s A pointer to a GlobalState, holding the current state of the program
 * @return int 
 */
extern int doEditorFrame(Editor *e, GlobalState *s);

/**
 * @brief Draws the Editor.
 * 
 * @param g A pointer to the Editor to render
 * @param s A pointer to the GlobalState
 * @param screen A pointer to the screen on which to render the Editor
 * @param bgColour The background colour of the Editor
 * @param fgColour The foreground colour of the Editor
 */
extern void drawEditor(Editor *e, GlobalState *s, C3D_RenderTarget *screen, u32 bgColour, u32 fgColour);

#endif