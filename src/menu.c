#include <stdio.h>
#include "menu.h"
#include "board.h"

void displayMainMenu() {
    printf("----- SUDOKU -----\n");
    printf("[1] START\n");
    printf("[2] INFO\n");
    printf("[0] QUIT\n");
}

void displayStartMenu(BoardSize* size, int* difficulty) {
    printf("----- SUDOKU -----\n");
    printf("[1] START\n");
    printf("[2] LOAD GAME\n");
    printf("[3] CHANGE SIZE ( %ix%i )\n", (int)*size, (int)*size);
    printf("[4] CHANGE DIFFICULTY ( %i%% empty cells )\n", *difficulty);
    printf("[0] BACK\n");
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