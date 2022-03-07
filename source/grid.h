#ifndef _GRID_H_GUARD_
#define _GRID_H_GUARD_

#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <citro2d.h>
#include <3ds.h>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

typedef struct {
	uint16_t size;
	float cellSize;
	char *cells;
} Grid;

extern Grid *newEmptyGrid(float cellSize);
extern void fillGridRandom(Grid *grid);
extern void destroyGrid(Grid *grid);
extern void drawGrid(Grid * grid, u32 colour);
extern char isAlive(char cell);
extern void killCell(Grid *grid, uint16_t index);
extern void newCell(Grid *grid, uint16_t index);
extern float randomFloat();
extern uint32_t getCoords(Grid *grid, uint16_t index);
extern uint32_t getIndex(Grid *grid, uint32_t coords);
extern char numberOfNeighbours(char cell);
extern void checkNeighbours(Grid *grid, uint16_t index);
extern void updateCell(Grid *grid, uint16_t index);
extern void updateGrid(Grid *grid);

#endif