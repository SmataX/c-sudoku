#ifndef MENU_H
#define MENU_H

#include "board.h"

// Display in terminal main menu options
void displayMainMenu();

// Display in terminal start menu options
void displayStartMenu(BoardSize* size, int* difficulty);

BoardSize selectBoardSize();
int selectDifficulty();

// Get user input between given range
int userInput(int min, int max);

#endif