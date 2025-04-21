#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "board.h"

// Check if placing a value does not violate subgrid constraints
int isSubgridValid(int** board, BoardSize size, int row, int col, int num) {
  int subgridSize = sqrt(size);
  int startRow = row - row % subgridSize;
  int startCol = col - col % subgridSize;

  for (int i = 0; i < subgridSize; i++) {
    for (int j = 0; j < subgridSize; j++) {
      if (board[startRow + i][startCol + j] == num) {
        return 0;
      }
    }
  }
  return 1;
}

// Check if placing a value does not violate row constraints
int isRowValid(int** board, BoardSize size, int row, int col, int num) {
  for (int i = 0; i < (int)size; i++) {
    if (board[row][i] == num && i != col) {
      return 0;
    }
  }
  return 1;
}

// Check if placing a value does not violate column constraints
int isColValid(int** board, BoardSize size, int row, int col, int num) {
  for (int i = 0; i < (int)size; i++) {
    if (board[i][col] == num && i != row) {
      return 0;
    }
  }
  return 1;
}

// Check if placing a value is valid (row, column, and subgrid)
int isBoardValid(int** board, BoardSize size, int row, int col, int num) {
  return isRowValid(board, size, row, col, num) *
         isColValid(board, size, row, col, num) *
         isSubgridValid(board, size, row, col, num);
}

// Fill a subgrid with random valid values
void fillSubgrid(int** board, BoardSize size, int row, int col) {
  int num;
  int subgridSize = sqrt(size);
  for (int i = 0; i < subgridSize; i++) {
    for (int j = 0; j < subgridSize; j++) {
      do {
        num = rand() % size + 1;
      } while (!isSubgridValid(board, size, row, col, num));
      board[row + i][col + j] = num;
    }
  }
}

// Recursively fill the remaining cells of the board with valid values
int fillRemaining(int** board, int size, int i, int j) {
  if (i == size) {
    return 1;
  }

  if (j == size) {
    return fillRemaining(board, size, i + 1, 0);
  }

  if (board[i][j] != 0) {
    return fillRemaining(board, size, i, j + 1);
  }

  for (int num = 1; num <= size; num++) {
    if (isBoardValid(board, size, i, j, num)) {
      board[i][j] = num;
      if (fillRemaining(board, size, i, j + 1)) {
        return 1;
      }
      board[i][j] = 0;
    }
  }

  return 0;
}

// Generate a complete Sudoku board
int** generateBoard(BoardSize size) {
  int **board = malloc(size * sizeof(int *));
  for (int i = 0; i < (int)size; i++) {
    board[i] = malloc(size * sizeof(int));
  }

  for (int i = 0; i < (int)size; i = i + sqrt(size)) {
    fillSubgrid(board, size, i, i);
  }

  fillRemaining(board, size, 0, 0);

  return board;
}

// Remove a given percentage of values from the board to create a puzzle
void emptyBoard(int** board, BoardSize size, int emptyPercent) {
  int toRemove = round((double)(size * size) * emptyPercent / 100);

  while (toRemove > 0) {
    int randX = rand() % (size);
    int randY = rand() % (size);

    if (board[randY][randX] != 0) {
      board[randY][randX] = 0;
      toRemove--;
    }
  }
}

// Create a deep copy of a Sudoku board (2D array)
int** copyBoard(int** source, int size) {
  int** dest = malloc(size * sizeof(int*));
  for (int i = 0; i < size; i++) {
      dest[i] = malloc(size * sizeof(int));
      for (int j = 0; j < size; j++) {
          dest[i][j] = source[i][j];
      }
  }
  return dest;
}

// Check if the Sudoku board is completely and correctly filled
int isBoardComplete(int** board, BoardSize size) {
  for (int i = 0; i < (int)size; i++) {
    for (int j = 0; j < (int)size; j++) {
      int num = board[i][j];
      if (num == 0) {
        return 0;
      }

      board[i][j] = 0;

      if (!isBoardValid(board, size, i, j, num)) {
        board[i][j] = num;
        return 0;
      }

      board[i][j] = num;
    }
  }

  return 1;
}

// Free memory allocated for 2D array
void freeBoard(int** board, BoardSize size) {
  for (int i = 0; i < (int)size; i++) {
    free(board[i]);
  }
  free(board);
}