#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "solver.h"

// Calculate energy (number of conflicts)
int calculateEnergy(int** board, BoardSize size) {
    int conflicts = 0;
    int n = (int)size;

    // Check row conflicts
    for (int i = 0; i < n; i++) {
        int* count = (int*)calloc(n + 1, sizeof(int));
        for (int j = 0; j < n; j++) {
            if (board[i][j] != 0) count[board[i][j]]++;
        }
        for (int k = 1; k <= n; k++) {
            if (count[k] > 1) conflicts += (count[k] - 1);
        }
        free(count);
    }

    // Check column conflicts
    for (int j = 0; j < n; j++) {
        int* count = (int*)calloc(n + 1, sizeof(int));
        for (int i = 0; i < n; i++) {
            if (board[i][j] != 0) count[board[i][j]]++;
        }
        for (int k = 1; k <= n; k++) {
            if (count[k] > 1) conflicts += (count[k] - 1);
        }
        free(count);
    }

    return conflicts;
}

// Generate a neighbor state by swapping two cells in the same subgrid
void generateNeighbor(int** board, int** fixedMask, BoardSize size) {
    int n = (int)size;
    int subgridSize = (n == 4) ? 2 : (n == 9) ? 3 : 4;

    // Pick a random subgrid
    int subRow = rand() % subgridSize;
    int subCol = rand() % subgridSize;
    int startRow = subRow * subgridSize;
    int startCol = subCol * subgridSize;

    // Find all non-fixed cells in the subgrid
    int candidates[n * 2]; // Stores [row1, col1, row2, col2, ...]
    int count = 0;

    for (int i = startRow; i < startRow + subgridSize; i++) {
        for (int j = startCol; j < startCol + subgridSize; j++) {
            if (fixedMask[i][j] == 0) {
                candidates[count++] = i;
                candidates[count++] = j;
            }
        }
    }

    if (count >= 4) { // At least two cells to swap
        int idx1 = (rand() % (count / 2)) * 2;
        int idx2;
        do {
            idx2 = (rand() % (count / 2)) * 2;
        } while (idx1 == idx2);

        int row1 = candidates[idx1], col1 = candidates[idx1 + 1];
        int row2 = candidates[idx2], col2 = candidates[idx2 + 1];

        // Swap values
        int temp = board[row1][col1];
        board[row1][col1] = board[row2][col2];
        board[row2][col2] = temp;
    }
}

// Solve the Sudoku puzzle using SA
int** solveSimulatedAnnealing(int** board, BoardSize size, int initialTemp, double coolingRate, int maxIterations) {
    int n = (int)size;
    int** currentBoard = copyBoard(board, n);
    int** fixedMask = generateFixedMask(board, size);

    // Initialize board with valid subgrids
    int subgridSize = (n == 4) ? 2 : (n == 9) ? 3 : 4;
    for (int i = 0; i < n; i += subgridSize) {
        for (int j = 0; j < n; j += subgridSize) {
            fillSubgrid(currentBoard, size, i, j);
        }
    }

    int currentEnergy = calculateEnergy(currentBoard, size);
    double temp = initialTemp;

    for (int iter = 0; iter < maxIterations && currentEnergy > 0; iter++) {
        int** neighborBoard = copyBoard(currentBoard, n);
        generateNeighbor(neighborBoard, fixedMask, size);
        int neighborEnergy = calculateEnergy(neighborBoard, size);

        int delta = neighborEnergy - currentEnergy;
        if (delta < 0 || (rand() / (double)RAND_MAX) < exp(-delta / temp)) {
            freeBoard(currentBoard, size);
            currentBoard = neighborBoard;
            currentEnergy = neighborEnergy;
        } else {
            freeBoard(neighborBoard, size);
        }

        temp *= coolingRate;
        
        printf("Iter: %d | Energy: %d | Temp: %.2f\n", iter, currentEnergy, temp);
    }

    freeBoard(fixedMask, size);
    return currentBoard;
}