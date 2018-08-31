#include "sudoku_board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliary_functions.h"
#include "doubly_linked_list.h"
#include "SPBufferset.h"


void initAsDefaultBoard(Board* board) {
	int oldEdgeSize = board->edgeSize;
	board->blockHeight = 3;
	board->blockLength = 3;
	board->edgeSize = 9;
	initMatrix(board, 0, oldEdgeSize);
	initMatrix(board, 1, oldEdgeSize);
	board->markErrors = 1;
	clearList(board->movesList);
	board->currNode = NULL;
}
void printRow(Board* board, int rowIdx) {
	int i;
	printf("|");
	for (i=0; i < board->edgeSize; i++) {
		printCell(board,rowIdx,i);
		if ((i+1)%board->blockLength == 0) {
			printf("|");
		}
	}
	printf("\n");
}

void printSeperatorRow(Board* board) {
	int i;
	for (i=0; i < (4*board->edgeSize + board->blockHeight +1); i++) {
		printf("-");
	}
	printf("\n");
}

void printBoard(Board* board){
/*board is NxN
 * n-by-m blocks (n rows of blocks, m columns of blocks)
 * board printing format has N+m+1 rows*/
	int i,j;
	printSeperatorRow(board);
	for (i=0 ; i<board->blockLength ; i++) {
		for (j=0 ; j<board->blockHeight ; j++) {
			printRow(board,i*board->blockHeight+j);
		}
		printSeperatorRow(board);
	}
}

void copyMatrix(int** orig, int** new, int size){
	int i,j;
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){
			new[i][j] = orig[i][j];
		}
	}
}
/*if boardMatrix = 1 it initializes board->matrix
 *else, it initializes board->isFixed */
void freeMatrix(Board* board, int boardMatrix, int oldEdgeSize) {
	int i,numOfRows;
	if (oldEdgeSize == 0) {
		numOfRows = board->edgeSize;
	}
	else {
		numOfRows = oldEdgeSize;
	}
	for (i=0 ; i<numOfRows ; i++) {
		if (boardMatrix) {
			free(board->matrix[i]);
		}
		else {
			free(board->isFixed[i]);
		}
	}
	if (boardMatrix) {
		free(board->matrix);
	}
	else {
		free(board->isFixed);
	}
}
/*if boardMatrix = 1 it initializes board->matrix
 *else, it initializes board->isFixed */
int initMatrix(Board* board, int boardMatrix, int oldEdgeSize) {
	int i;
	int** matrix = NULL;
	/*allocate memory for the board's matrix*/
	matrix = calloc(board->edgeSize, sizeof(int*));
	if (matrix == NULL) {
		return 999; /*if allocation failed, returns NULL*/
	}
	for(i=0 ;i < board->edgeSize; i++) {
		matrix[i] = calloc(board->edgeSize, sizeof(int));
	}
	if (boardMatrix) {
		if(board->matrix != NULL) {
			freeMatrix(board,1,oldEdgeSize);
		}

		board->matrix = matrix;
	}
	else {
		if(board->isFixed != NULL) {
			freeMatrix(board,0,oldEdgeSize);
		}
		board->isFixed = matrix;
	}
	return 1;
}

Board* createBoard(int blockHeight, int blockLength) {
	int allocationError1, allocationError2;
	int boardSize = blockHeight*blockLength;
	Board* board;

	board = (Board *) calloc(boardSize,sizeof(Board));
	if (board == NULL) {
		return NULL; /*if allocation failed, returns NULL*/
	}


	/*TODO When writing main function decide if to do markErrors = 1 in createBoard or independently in main function
	 * or keep it here in createBoard */
	board->markErrors = 1;
	board->blockHeight=blockHeight;
	board->blockLength=blockLength;
	board->edgeSize=boardSize;
	board->movesList = createEmptyList();
	/*initializes board->matrix*/
	allocationError1 = initMatrix(board,1,board->edgeSize);
	/*initializes board->isFixed*/
	allocationError2 = initMatrix(board,0,board->edgeSize);


	if (allocationError1 == 999 || allocationError2 == 999) {
		return NULL;
	}


	return board;
}

/*creates a partial copy of board (only some parameters are copied:
 * blockHeight, blockLength, edgeSize, matrix, isFixed*/
Board* copyBoard(Board* board){
	Board* copied = createBoard(board->blockHeight, board->blockLength);

	if (copied == NULL) {
		return NULL; /*if allocation failed, returns NULL*/
	}
	copyMatrix(board->matrix,copied->matrix,board->edgeSize);
	copyMatrix(board->isFixed, copied->isFixed, board->edgeSize);

	return copied;
}


void destroyBoard(Board* board){
	freeMatrix(board,1,board->edgeSize);
	freeMatrix(board,0,board->edgeSize);
	freeList(board->movesList);
	free(board);
}



void printCell(Board* board, int i, int j) {
	if (board->matrix[i][j] == 0) {
		printf("   ");
	}
	else {
		printf(" %2d", board->matrix[i][j]);
	}
	if (board->isFixed[i][j]) {
		printf(".");
	}
	else if (board->mode == 2 || board->markErrors == 1) {
		if (isCellErr(board,i,j)) {
			printf("*");
		}
		else {
			printf(" ");
		}
	}
}

int isBoardErr(Board* board) {
	int i,j;
	for (i=0 ; i<board->edgeSize ; i++) {
		for (j=0 ; j<board->edgeSize ; j++) {
			if (isCellErr(board,i,j)) {
				return 1;
			}
		}
	}
	return 0;
}


int numberOfFilledCells(Board* board) {
/*returns the number of filled cells*/
	int i,j;
	int filledNum=0;
	for(i=0; i<board->edgeSize;i++){
		for(j=0;j < board->edgeSize;j++){
			if (board->matrix[i][j] != 0) {
				filledNum++;
			}
		}
	}
	return filledNum;
}

int numberOfBlankCells(Board* board) {
	return board->edgeSize*board->edgeSize - numberOfFilledCells(board);
}


/* Input:
 * A square matrix and its edge size
 *
 * Output:
 * None
 *
 * Description:
 * Sets all the values of matrix to 0*/

void clearMatrix(int** matrix, int edgeSize) {
	int i,j;
	for (i=0 ; i<edgeSize ; i++) {
		for (j=0 ; j<edgeSize ; j++) {
			matrix[i][j] = 0;
		}
	}
}
