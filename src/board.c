#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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
// In board.c
void fillSubgrid(int** board, BoardSize size, int startRow, int startCol) {
  int n = (int)size;
  int subgridSize = (n == 4) ? 2 : (n == 9) ? 3 : 4;
  int nums[n];
  for (int i = 0; i < n; i++) nums[i] = i + 1;

  // Shuffle numbers
  for (int i = 0; i < n; i++) {
      int j = rand() % n;
      int temp = nums[i];
      nums[i] = nums[j];
      nums[j] = temp;
  }

  int idx = 0;
  for (int i = startRow; i < startRow + subgridSize; i++) {
      for (int j = startCol; j < startCol + subgridSize; j++) {
          if (board[i][j] == 0) {  // Only fill empty cells
              board[i][j] = nums[idx++];
          } else {
              idx++;  // Skip fixed cells
          }
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

// Save board data to file
void saveToFile(int** board, int** gameBoard, BoardSize size) {
  FILE* file = fopen("save.txt", "w");
  fprintf(file, "%i\n", (int)size);

  // Write empty board
  for (int i = 0; i < (int)size; i++) {
    for (int j = 0; j < (int)size; j++) {
      fprintf(file, "%i ", board[i][j]);
    } 
  }

  fprintf(file, "\n");
  
  // Write user-filled board
  for (int i = 0; i < (int)size; i++) {
    for (int j = 0; j < (int)size; j++) {
      fprintf(file, "%i ", gameBoard[i][j]);
    } 
  }

  fclose(file);
}

// Loads Sudoku board data from "save.txt" into provided buffers
void loadFromFile(int** board, int** gameBoard, BoardSize* size) {
  FILE* file = fopen("save.txt", "r");

  if (file == NULL) {
    printf("No game saved!");

    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
    getchar();
    exit(0);
  }

  char line[300];
  if (fgets(line, 300, file)) {
    *size = atoi(line);
  }

  if (fgets(line, 300, file)) {
    parseLine(line, board, *size);
  }

  if (fgets(line, 300, file)) {
    parseLine(line, gameBoard, *size);
  }
  fclose(file);
}

void parseLine(char* line, int** board, int size) {
  char* token = strtok(line, " \n");
  for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
          if (!token) {
              printf("Error: Not enough values in line!\n");
              exit(1);
          }
          board[i][j] = atoi(token);
          token = strtok(NULL, " \n");
      }
  }
}

int** generateFixedMask(int** puzzle, BoardSize size) {
  int n = (int)size;
  int** mask = (int**)malloc(n * sizeof(int*));
  for (int i = 0; i < n; i++) {
      mask[i] = (int*)malloc(n * sizeof(int));
      for (int j = 0; j < n; j++) {
          mask[i][j] = (puzzle[i][j] != 0) ? 1 : 0;
      }
  }
  return mask;
}