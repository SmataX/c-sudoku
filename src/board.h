#ifndef BOARD_H
#define BOARD_H

// Predefined board sizes
typedef enum BoardSize { SMALL = 4, MEDIUM = 9, LARGE = 16 } BoardSize;

// Check if placing a value does not violate subgrid constraints
int isSubgridValid(int** board, BoardSize size, int row, int col, int num);

// Check if placing a value does not violate row constraints
int isRowValid(int** board, BoardSize size, int row, int col, int num);

// Check if placing a value does not violate column constraints
int isColValid(int** board, BoardSize size, int row, int col, int num);

// Check if placing a value is valid (row, column, and subgrid)
int isBoardValid(int** board, BoardSize size, int row, int col, int num);

// Fill a subgrid with random valid values
void fillSubgrid(int** board, BoardSize size, int row, int col);

// Recursively fill the remaining cells of the board with valid values
int fillRemaining(int** board, int size, int i, int j);

// Generate a complete Sudoku board
int** generateBoard(BoardSize size);

// Remove a given percentage of values from the board to create a puzzle
void emptyBoard(int** board, BoardSize size, int emptyPercent);

// Create a deep copy of a Sudoku board (2D array)
int** copyBoard(int** source, int size);

// Check if the Sudoku board is completely and correctly filled
int isBoardComplete(int** board, BoardSize size);

// Free memory allocated for 2D array
void freeBoard(int** board, BoardSize size);

// Save board data to file
void saveToFile(int** board, int** gameBoard, BoardSize size, int difficulty);

// Loads Sudoku board data from "save.txt" into provided buffers
void loadFromFile(int** board, int** gameBoard, BoardSize* size);

// Convert string line to board
void parseLine(char* line, int** board, int size);

#endif
