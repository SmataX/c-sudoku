#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "board.h"
#include "menu.h"

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
int moveCursor(int* cursorX, int* cursorY, int** board, int** startBoard, BoardSize size) {
  char userCommand[10];
  
  printf("Enter direction (w - up   s - down   a - left   d - right) or number\n");
  printf("You can save your game with save command, to exit write exit \n");
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
  } else if (strcmp(userCommand, "save") == 0) {
    return 1;
  } else if (strcmp(userCommand, "exit") == 0) {
    return 2;
  }
  else {
      if (startBoard[*cursorY][*cursorX] == 0) {
          int number = atoi(userCommand);

          if (number >= 1 && number <= (int)size) {
              board[*cursorY][*cursorX] = number;
          }
      }
  }

  return 0;
}

void initGame(BoardSize size, int difficulty) {
  // Cursor position
  int cursorX = 0, cursorY = 0;

  // Generate and prepare board
  int** board = generateBoard(size);
  emptyBoard(board, size, difficulty);
  int** gameplayBoard = copyBoard(board, size);

  // Game loop
  while (1) {
    printf("\033[H\033[J"); // Clear terminal
    displayBoard(gameplayBoard, board, size, cursorX, cursorY);
    int option = moveCursor(&cursorX, &cursorY, gameplayBoard, board, size);

    switch (option) {
      // Save progress
      case 1: break;
      case 2: exit(0);
      default: break;
    }

    if (isBoardComplete(gameplayBoard, size)) {
      break;
    }
  }

  // Message after completing sudoku
  printf("\033[H\033[J");
  displayBoard(gameplayBoard, board, size, cursorX, cursorY);
  printf("You completed a sudoku!");


  freeBoard(board, size);
  freeBoard(gameplayBoard, size);
}

int main(void) {
  srand(time(NULL));

  // Sudoku settings
  BoardSize boardSize = MEDIUM;
  int emptyPercentage = 50;

  // Game menu
  int userInput, userInput2;

  // Main loop
  while (1) {
    // Main Menu
    printf("\033[H\033[J"); // Clear terminal
    displayMainMenu();
    printf("Enter your choice\n");
    printf(" > ");
    scanf("%d", &userInput);

    switch (userInput) {
      // Quit Game
      case 0: exit(0);

      // Start Game Menu
      case 1: 
        while(1) {
          printf("\033[H\033[J"); // Clear terminal
          displayStartMenu(&boardSize, &emptyPercentage);
          printf("Enter your choice\n");
          printf(" > ");
          scanf("%d", &userInput2);
  
          switch (userInput2) {
            case 0: 
              goto exit_loop; 
              break;
            case 1: 
              initGame(boardSize, emptyPercentage); 
              break;
            case 2: break;
            case 3: 
              printf("\033[H\033[J");
              boardSize = selectBoardSize(); 
              break;
            case 4: 
              printf("\033[H\033[J");
              emptyPercentage = selectDifficulty(); 
              break;
          }
        }
        exit_loop: ;
        break;
        
      // Stats Menu
      case 2: break;
    }
  }
  
  return 0;
}
