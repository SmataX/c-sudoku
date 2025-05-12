#ifndef SOLVER_H
#define SOLVER_H

#include "board.h"

// Calculate energy (number of conflicts)
int calculateEnergy(int** board, BoardSize size);

// Generate a neighbor state by swapping two cells in the same subgrid
void generateNeighbor(int** board, int** fixedMask, BoardSize size);

// Solve the Sudoku puzzle using SA
int** solveSimulatedAnnealing(int** board, BoardSize size, int initialTemp, double coolingRate, int maxIterations);

#endif