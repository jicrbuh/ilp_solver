/*
 * sudoku_board.h
 *
 *  Created on: 14 Jul 2018
 *      Author: chen
 */

#ifndef SUDOKU_BOARD_H_
#define SUDOKU_BOARD_H_
#include "doubly_linked_list.h"
#include "SPBufferset.h"


typedef struct board_t {

	int blockHeight;

	int blockLength;

	int edgeSize;

	/* 0 - init, 1 - solve, 2 - edit*/
	int mode;

	/* 1 - errors are marked, 0 - errors are not marked*/
	int markErrors;

	/* the number of solutions to the board according to the last call to num_solutions command*/
	int lastNumOfSolutions;

	/* last hint given */
	int lastHint;

	/* the matrix that represents the sudoku board*/
	int** matrix;

	/* a matrix with the dimensions of the board which contains is each cell:
	 * 1 - if the cell is fixed, 0 - otherwise*/
	int** isFixed;

	/* points to the current state of the sudoku board according to the doubly linked list */
	Node* currNode;

	/* a doubly linked list of type Node* that describes the moves */
	DLL* movesList;

} Board;

/* TODO documentation update for sudoku_board.h
 * creates an empty Board with block_height and block_length.
 * the board is initialized to '0'
 * returns a pointer to the board, or NULL if memory allocation failed
 * */
Board* createBoard(int block_height, int block_length);
Board* copyBoard(Board* board);
Board* deepCopy(Board* board);
void destroyBoard(Board* board);
void printBoard(Board* board);
void clearBoard(Board* board);
void resizeMatrices(Board* board);
int numberOfBlankCells(Board* board);
int numberOfFilledCells(Board* board);
int initMatrix(Board* board, int boardMatrix, int oldEdgeSize);
void printSeparatorRow(Board* board);
void printCellRow(Board* board, int rowIdx);
void printCell(Board* board, int i, int j);
void fixAll(Board* board);
void copyMatrix(int** orig, int** new, int size);
int isBoardErr(Board* board);
void initAsDefaultBoard(Board* board);
void clearMatrix(int** matrix, int edgeSize);

#endif /* SUDOKU_BOARD_H_ */
