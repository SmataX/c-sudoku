#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "board.h"

// GA Parameters
#define POPULATION_SIZE 1000
#define MAX_GENERATIONS 10000
#define CROSSOVER_RATE 0.8
#define MUTATION_RATE 0.1
#define TOURNAMENT_SIZE 5

typedef struct {
    int **board;    // Sudoku board
    int fitness;    // Fitness score (lower is better)
} Individual;

// Calculate fitness (number of constraint violations)
int computeFitness(int **board, BoardSize size) {
    int n = size;
    int total_duplicates = 0;
    int subgrid_size = (int)sqrt(n);

    // Check columns
    for (int j = 0; j < n; j++) {
        int *freq = (int*)calloc(n + 1, sizeof(int));
        for (int i = 0; i < n; i++) {
            if (board[i][j] != 0) {
                freq[board[i][j]]++;
            }
        }
        for (int num = 1; num <= n; num++) {
            if (freq[num] > 1) {
                total_duplicates += (freq[num] - 1);
            }
        }
        free(freq);
    }

    // Check subgrids
    for (int i0 = 0; i0 < n; i0 += subgrid_size) {
        for (int j0 = 0; j0 < n; j0 += subgrid_size) {
            int *freq = (int*)calloc(n + 1, sizeof(int));
            for (int i = i0; i < i0 + subgrid_size; i++) {
                for (int j = j0; j < j0 + subgrid_size; j++) {
                    if (board[i][j] != 0) {
                        freq[board[i][j]]++;
                    }
                }
            }
            for (int num = 1; num <= n; num++) {
                if (freq[num] > 1) {
                    total_duplicates += (freq[num] - 1);
                }
            }
            free(freq);
        }
    }
    return total_duplicates;
}

// Initialize an individual with the puzzle and random permutations
void initializeIndividual(Individual *ind, int **puzzle, int **fixedMask, BoardSize size) {
    int n = size;
    ind->board = createBoard(size);
    copyBoardTo(ind->board, puzzle, size);

    for (int i = 0; i < n; i++) {
        int *present = (int*)calloc(n + 1, sizeof(int));
        int *row = ind->board[i];
        int *fixedRow = fixedMask[i];

        for (int j = 0; j < n; j++) {
            if (fixedRow[j] || row[j] != 0) {
                present[row[j]] = 1;
            }
        }

        int *missing = (int*)malloc(n * sizeof(int));
        int count = 0;
        for (int num = 1; num <= n; num++) {
            if (!present[num]) {
                missing[count++] = num;
            }
        }

        for (int j = 0; j < count; j++) {
            int r = j + rand() % (count - j);
            int temp = missing[j];
            missing[j] = missing[r];
            missing[r] = temp;
        }

        int idx = 0;
        for (int j = 0; j < n; j++) {
            if (!fixedRow[j]) {
                row[j] = missing[idx++];
            }
        }
        free(present);
        free(missing);
    }
    ind->fitness = computeFitness(ind->board, size);
}

// Tournament selection (minimization)
Individual tournamentSelection(Individual *population) {
    Individual best = population[rand() % POPULATION_SIZE];
    for (int i = 1; i < TOURNAMENT_SIZE; i++) {
        Individual candidate = population[rand() % POPULATION_SIZE];
        if (candidate.fitness < best.fitness) {
            best = candidate;
        }
    }
    return best;
}

// Row-based crossover
void crossover(Individual *parent1, Individual *parent2, Individual *child1, Individual *child2, BoardSize size) {
    int n = size;
    int crossover_point = rand() % (n - 1) + 1;
    for (int i = 0; i < crossover_point; i++) {
        for (int j = 0; j < n; j++) {
            child1->board[i][j] = parent1->board[i][j];
            child2->board[i][j] = parent2->board[i][j];
        }
    }
    for (int i = crossover_point; i < n; i++) {
        for (int j = 0; j < n; j++) {
            child1->board[i][j] = parent2->board[i][j];
            child2->board[i][j] = parent1->board[i][j];
        }
    }
}

// Mutation: swap two non-fixed cells in a row
void mutate(Individual *ind, int **fixedMask, BoardSize size) {
    int n = size;
    int row = rand() % n;
    int non_fixed_count = 0;
    int *non_fixed_cols = (int*)malloc(n * sizeof(int));

    for (int j = 0; j < n; j++) {
        if (!fixedMask[row][j]) {
            non_fixed_cols[non_fixed_count++] = j;
        }
    }

    if (non_fixed_count >= 2) {
        int idx1 = rand() % non_fixed_count;
        int idx2;
        do {
            idx2 = rand() % non_fixed_count;
        } while (idx1 == idx2);
        int col1 = non_fixed_cols[idx1];
        int col2 = non_fixed_cols[idx2];
        int temp = ind->board[row][col1];
        ind->board[row][col1] = ind->board[row][col2];
        ind->board[row][col2] = temp;
    }
    free(non_fixed_cols);
}

// Free an individual's board
void freeIndividual(Individual *ind, BoardSize size) {
    freeBoard(ind->board, size);
}

// Genetic Algorithm to solve Sudoku
int** solveSudokuGA(int** puzzle, BoardSize size) {
    srand(time(NULL));
    int **fixedMask = generateFixedMask(puzzle, size);
    Individual *population = malloc(POPULATION_SIZE * sizeof(Individual));
    Individual *next_population = malloc(POPULATION_SIZE * sizeof(Individual));

    // Initialize population
    for (int i = 0; i < POPULATION_SIZE; i++) {
        initializeIndividual(&population[i], puzzle, fixedMask, size);
    }

    int best_fitness = -1;
    Individual *best_individual = NULL;

    for (int generation = 0; generation < MAX_GENERATIONS; generation++) {
        // Find best individual
        int best_idx = 0;
        for (int i = 1; i < POPULATION_SIZE; i++) {
            if (population[i].fitness < population[best_idx].fitness) {
                best_idx = i;
            }
        }

        if (population[best_idx].fitness == 0) {
            best_individual = &population[best_idx];
            break;
        }

        // Elitism: copy best individual to new population
        next_population[0].board = copyBoard(population[best_idx].board, size);
        next_population[0].fitness = population[best_idx].fitness;

        int current_index = 1;
        while (current_index < POPULATION_SIZE) {
            Individual parent1 = tournamentSelection(population);
            Individual parent2 = tournamentSelection(population);

            Individual child1, child2;
            child1.board = createBoard(size);
            child2.board = createBoard(size);

            if ((double)rand() / RAND_MAX < CROSSOVER_RATE) {
                crossover(&parent1, &parent2, &child1, &child2, size);
            } else {
                parent1.board = copyBoard(child1.board, size);
                parent2.board = copyBoard(child2.board, size);
                // copyBoardTo(child1.board, parent1.board, size);
                // copyBoardTo(child2.board, parent2.board, size);
            }

            if ((double)rand() / RAND_MAX < MUTATION_RATE) {
                mutate(&child1, fixedMask, size);
            }
            if ((double)rand() / RAND_MAX < MUTATION_RATE) {
                mutate(&child2, fixedMask, size);
            }

            child1.fitness = computeFitness(child1.board, size);
            child2.fitness = computeFitness(child2.board, size);

            next_population[current_index++] = child1;
            if (current_index < POPULATION_SIZE) {
                next_population[current_index++] = child2;
            } else {
                freeBoard(child2.board, size);
            }
        }

        // Replace old population
        for (int i = 0; i < POPULATION_SIZE; i++) {
            freeBoard(population[i].board, size);
            population[i] = next_population[i];
        }
    }

    // Cleanup and return solution
    int **solution = NULL;
    if (best_individual != NULL) {
        solution = copyBoard(best_individual->board, size);
    } else {
        // Fallback: best from last generation
        int best_idx = 0;
        for (int i = 1; i < POPULATION_SIZE; i++) {
            if (population[i].fitness < population[best_idx].fitness) {
                best_idx = i;
            }
        }
        solution = copyBoard(population[best_idx].board, size);
    }

    for (int i = 0; i < POPULATION_SIZE; i++) {
        freeBoard(population[i].board, size);
    }
    free(population);
    free(next_population);
    freeBoard(fixedMask, size);
    return solution;
}

// Print the Sudoku board
void printBoard(int** board, BoardSize size) {
    int n = size;
    int subgrid_size = (int)sqrt(n);
    for (int i = 0; i < n; i++) {
        if (i % subgrid_size == 0 && i != 0) {
            for (int j = 0; j < n + subgrid_size - 1; j++) {
                printf("--");
            }
            printf("\n");
        }
        for (int j = 0; j < n; j++) {
            if (j % subgrid_size == 0 && j != 0) {
                printf("| ");
            }
            printf("%2d", board[i][j]);
        }
        printf("\n");
    }
}

// Main function to demonstrate the GA solver
int main() {
    // Example 9x9 Sudoku puzzle (0 represents empty cells)
    BoardSize size = MEDIUM;
    int puzzle[9][9] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    // Convert puzzle to dynamic array
    int **dynamicPuzzle = generateBoard(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            dynamicPuzzle[i][j] = puzzle[i][j];
        }
    }

    printf("Original Puzzle:\n");
    printBoard(dynamicPuzzle, size);

    // Solve the puzzle
    int **solution = solveSudokuGA(dynamicPuzzle, size);

    if (solution != NULL) {
        printf("\nSolution:\n");
        printBoard(solution, size);
        freeBoard(solution, size);
    } else {
        printf("\nNo solution found within the generation limit.\n");
    }

    freeBoard(dynamicPuzzle, size);
    return 0;
}