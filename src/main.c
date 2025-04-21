#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "board.h"

// Get user input between given range
int userInput(int min, int max) {
  int input;
  do {
      if (scanf("%d", &input) != 1) {
          while (getchar() != '\n');
          printf("Invalid input. Try again.\n");
      }
  } while (input < min || input > max);

  return input;
}

BoardSize selectBoardSize() {
  printf("\n--- SELECT BOARD SIZE ---\n");
  printf("[1] SMALL  (4x4)\n");
  printf("[2] MEDIUM (9x9)\n");
  printf("[3] LARGE  (16x16)\n");
  printf("Select size: ");
  int option = userInput(1, 3);

  switch (option) {
    case 1: return SMALL;
    case 2: return MEDIUM;
    case 3: return LARGE;
    default: return MEDIUM;
  }
}

int selectDifficulty() {
  printf("\n--- SELECT DIFFICULTY ---\n");
  printf("[1] EASY   (25%% empty)\n");
  printf("[2] MEDIUM (50%% empty)\n");
  printf("[3] HARD   (75%% empty)\n");
  printf("Select difficulty: ");
  int option = userInput(1, 3);

  switch (option) {
    case 1: return 25;
    case 2: return 50;
    case 3: return 75;
    default: return 50;
  }
}

// Display sudoku in the terminal
void displayBoard(int** board, int** startBoard, BoardSize size, int cursorX, int cursorY) {
  int subgridSize = sqrt(size);

  for (int i = 0; i < (int)size; i++) {
    for (int j = 0; j < (int)size; j++) {

      // Highlight the selected cell
      if (cursorX == j && cursorY == i) {
        printf("\033[47m");
      }

      // Use different color for user-filled numbers
      if (startBoard[i][j] == 0) {
        printf("\033[1m\033[36m");
      }

      // Print the cell and reset formatting 
      if (board[i][j] == 0) {
        printf(".\033[0m");
      }
      else {
        printf("%d\033[0m", board[i][j]);
      }

      // Add spacing between numbers
      printf("%s", (board[i][j] <= 9) ? "  " : " ");
      
      // Add spacing between subgrids horizontally
      if ((j + 1) % subgridSize == 0) {
        printf("  ");    
      }
    }
    printf("\n");

    // Add spacing between subgrids vertically
    if ((i + 1) % subgridSize == 0) {
      printf("\n");
    }
  }
}

// User input for moving cursor and entering numbers
void moveCursor(int* cursorX, int* cursorY, int** board, int** startBoard, BoardSize size) {
  char userCommand[10];
  
  printf("Enter direction (w - up   s - down   a - left   d - right) or number\n");
  printf(" > ");
  scanf("%s", userCommand);
  
  if (strcmp(userCommand, "w") == 0) {
      if (*cursorY > 0) {
          (*cursorY)--;
      }
  } else if (strcmp(userCommand, "s") == 0) {
      if (*cursorY < (int)size - 1) {
          (*cursorY)++;
      }
  } else if (strcmp(userCommand, "a") == 0) {
      if (*cursorX > 0) {
          (*cursorX)--;
      }
  } else if (strcmp(userCommand, "d") == 0) {
      if (*cursorX < (int)size - 1) {
          (*cursorX)++;
      }
  } else {
      if (startBoard[*cursorY][*cursorX] == 0) {
          int number = atoi(userCommand);

          if (number >= 1 && number <= (int)size) {
              board[*cursorY][*cursorX] = number;
          }
      }
  }
}

int main(void) {
  srand(time(NULL));

  int cursorX = 0, cursorY = 0;

  BoardSize boardSize = selectBoardSize();
  int emptyPercentage = selectDifficulty();

  // Generate and prepare board
  int** board = generateBoard(boardSize);
  emptyBoard(board, boardSize, emptyPercentage);
  int** gameplayBoard = copyBoard(board, boardSize);

  // Game loop
  while (1) {
    printf("\033[H\033[J"); // Clear terminal
    displayBoard(gameplayBoard, board, boardSize, cursorX, cursorY);
    moveCursor(&cursorX, &cursorY, gameplayBoard, board, boardSize);

    if (isBoardComplete(gameplayBoard, boardSize)) {
      break;
    }
  }

  // Message after completing sudoku
  printf("\033[H\033[J");
  displayBoard(gameplayBoard, board, boardSize, cursorX, cursorY);
  printf("You completed a sudoku!");


  freeBoard(board, boardSize);
  freeBoard(gameplayBoard, boardSize);

  
  return 0;
}
