/*
 * auxillary_functions.h
 *
 *  Created on: 3 Aug 2018
 *      Author: chen
 */

#ifndef AUXILLARY_FUNCTIONS_H_
#define AUXILLARY_FUNCTIONS_H_
#include "sudoku_board.h"


/*
 * Description:
 * Checks whether s contains only whitespace chars
 *
 * Input:
 * a string (char*)
 *
 * Output:
 * 1 if s contains only whitespace
 * 0 otherwise
 *
 * */
int isEmpty(char* s);


/*TODO TESTED
 *
 * Description:
 * Checks whether cell x,y is erroneous
 *
 * Input:
 * Board and cell coordinates - x for the row and y for the column
 *
 * Output:
 * 1 if cell x,y is erroneous
 * 0 otherwise
 *
 * */
int isCellErr(Board* board, int x, int y);


/*
 *
 * Description:
 * initializes a new stack
 *
 * Input:
 * Board and a doubly linked list (as the implementation for the stack)
 *
 * Output:
 * None
 *
 * */
void initStack(Board* board, DLL* stack);


/*
 *
 * Description:
 * Exhausts all possible values for all empty cells of the board, counting all solutions found in this process.
 * Once the algorithm backtracks from the first empty cell it returns the number of solutions found.
 *
 * Input:
 * The Sudoku board
 *
 * Output:
 * The number of solutions to the current Sudoku board
 *
 * */
int exBackTracking(Board* board);


/*TODO TESTED
 *
 * Description:
 * Loads a puzzle from a file by updating the Sudoku board struct fields
 *
 * Input:
 * A pointer to a file and the Sudoku board
 *
 * Output:
 * None
 *
 * */
void loadToBoard(FILE* fptr, Board* board);




void printSetUndoRedo(Board* board, int undo);

void printAutofillUndoRedo(Board* board, int undo);

void loadToBoard(FILE* fptr, Board* board);

void fillXRandomCells(Board* board, int x);

int ILPSolver(Board* board);

void eraseAllButYRandomCells(Board* board, int y);

/*
 *
 * Description:
 * Saves the Sudoku board to a file.
 * The first line consists of the block length and the block height.
 * Every line preceding the first line represents a line in the board,
 * where a fixed cell is marked with a dot
 *
 * Input:
 * A pointer to a file and the Sudoku board
 *
 * Output:
 * None
 *
 * */
void saveToFile(FILE* fptr,Board* board);

int cellHasOnePossibleValue(Board* board, int x, int y);

#endif /* AUXILLARY_FUNCTIONS_H_ */
