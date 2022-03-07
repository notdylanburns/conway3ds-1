#include "grid.h"

float randomFloat() {
	//Remember to use srand() before using this function in order to seed to random algorithm
	float r = rand();
	return r/RAND_MAX;
}

Grid *newEmptyGrid(float cellSize) {
	Grid *grid = malloc(sizeof(Grid));
	if (grid == NULL) return NULL;

	grid->cellSize = cellSize;

	//Calculate length of grid
	grid->size = (SCREEN_WIDTH * SCREEN_HEIGHT) / (grid->cellSize * grid->cellSize);

	//Create grid array
	grid->cells = malloc(grid->size);
	if (grid->cells == NULL) return NULL;

	//Fill grid array
	for (int i = 0; i < grid->size; i++) {
		grid->cells[i] = (char)0;
	}

	return grid;
}

void fillGridRandom(Grid *grid) {
	//Fill grid array
	float random;
	srand(time(0)); //Seed random algorithm with current time
	for (int i = 0; i < grid->size; i++) {
		random = randomFloat();
		if (random <= 0.5) {
			killCell(grid, i);
		} else {
			newCell(grid, i);
		}
	}
}

void destroyGrid(Grid *grid) {
	if (grid == NULL)
		return;
		
	free(grid->cells);
	free(grid);
}

void killCell(Grid *grid, uint16_t index) {
	grid->cells[index] = numberOfNeighbours(grid->cells[index]);
}

void newCell(Grid *grid, uint16_t index) {
	char bitmask = 0x10;
	grid->cells[index] = (grid->cells[index] | bitmask);
}

char isAlive(char cell) {
	char bitmask = 0x10;
	if (cell & bitmask) {
		return (char) 1;
	}
	return (char) 0;
}

char numberOfNeighbours(char cell) {
	char bitmask = 0x0F;
	return (cell & bitmask);
}

void checkNeighbours(Grid *grid, uint16_t index) {
	int width = SCREEN_WIDTH / grid->cellSize;

	unsigned char neighbours = 0;

	unsigned char cell;

	for (signed char x = -1; x <= 1; x++) {
		for (signed char y = -1; y <= 1; y++) {
			//Make sure that i and j cannot both be 0 otherwise it would check itself
			if (x == 0 && y == 0) continue;
			//Boundary conditions for the edge of the screen
			//X Boundary conditions
			if (x == -1 && index == 0) continue;
			if (x == -1 && index % width == 0) continue;
			if (x == 1 && ((index+1) % width == 0)) continue;
			//Y Boundary conditions
			if (y == -1 && index < width) continue;
			if (y == 1 && index+width > grid->size) continue;

			cell = grid->cells[index+x+(y*width)];
			if (isAlive(cell)) {
				neighbours += 1;
			}
		}
	}

	cell = grid->cells[index];
	//Clear last 4 bits containing neighbour data
	cell = (cell & 0xF0);
	//Replace last 4 bits with new neighbour data
	cell = (cell | neighbours);
	//Save to array
	grid->cells[index] = cell;
}

void updateCell(Grid *grid, uint16_t index) {
	char cell = grid->cells[index];
	char neighbours = numberOfNeighbours(cell);

	//Check for under or over population
	if (isAlive(cell)) {
		if (neighbours < 2 || neighbours > 3) {
			killCell(grid, index);
		}
	} else if (!isAlive(cell)) { //Check for reproduction if cell is dead
		if (neighbours == 3) {
			newCell(grid, index);
		}
	}
}

uint32_t getCoords(Grid *grid, uint16_t index) {
	//Returns x and y coordinates as a 32 bit integer with the first 16 bits being the x coordinate and the last 16 bits being the y coordinate
	int width = (int)(SCREEN_WIDTH / grid->cellSize);
	uint32_t x = (uint32_t)((index % width) * grid->cellSize) << 16;
	uint32_t y = (index / width) * grid->cellSize;
	uint32_t coords = x | y;
	return coords;
}

uint32_t getIndex(Grid *grid, uint32_t coords) {
	int width = SCREEN_WIDTH / grid->cellSize;
	uint16_t x = (0xFFFF0000 & coords) >> 16;
	uint16_t y = (0x0000FFFF & coords);
	uint32_t i = (uint32_t)((x/grid->cellSize) + (width*(y/grid->cellSize)));
	return i;
}

void drawGrid(Grid *grid, u32 colour) {
	uint16_t x;
	uint16_t y;
	for (uint16_t i = 0; i < grid->size; i++) {
		if (!isAlive(grid->cells[i])) continue;
		uint32_t coords = getCoords(grid, i);
		x = (0xFFFF0000 & coords) >> 16;
		y = (0x0000FFFF & coords);

		C2D_DrawRectSolid(x,y,0.0f,grid->cellSize,grid->cellSize,colour);
	}
}

void updateGrid(Grid *grid) {
	//Update Neighbours for every cell
	for (uint16_t i = 0; i < grid->size; i++) {
		checkNeighbours(grid, i);
	}
	//Update cell states
	for (uint16_t i = 0; i < grid->size; i++) {
		updateCell(grid, i);
	}
}