#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sudoku_board.h"
#include "doubly_linked_list.h"
#include <ctype.h>


/*returns 1 if all chars are whitespace*/
int isEmpty(char* s) {
  while (*s != '\0') {
    if (!isspace((unsigned char)*s))
      return 0;
    s++;
  }
  return 1;
}

int isColErr(Board* board, int x, int y){
	/*returns 1 if erroneous(relatively to row) or 0 else*/
	int i;
	for(i=0;i < board->edgeSize; i++) {
		if (board->matrix[x][i] == board->matrix[x][y] && i != y) {
			return 1;
		}
	}
	return 0;
}


int isRowErr(Board* board, int x, int y){
	/*returns 1 if erroneous (relatively to column) or 0 else*/
	int i;
		for(i=0;i < board->edgeSize; i++) {
			if (board->matrix[i][y] == board->matrix[x][y] && i != x) {
				return 1;
			}
		}
	return 0;
}

int isBlockErr(Board* board, int x, int y){
	/*returns 1 if erroneous (relatively to block) or 0 else*/
	int i,j;
	/*[iStart,jStart] is the upper left coordinate of the block*/
	int iStart = board->blockLength*(x/board->blockLength);
	int jStart = board->blockHeight*(y/board->blockHeight);

	for (i=0;i < board->blockLength; i++) {
		for (j=0; j< board->blockHeight ; j++) {
			if (board->matrix[x][y] == board->matrix[iStart+i][jStart+j] && x != (iStart+i) && y != (jStart+j)) {
				return 1;
			}
		}
	}
	return 0;
}

int isCellErr(Board* board, int x, int y) {
	/*returns 1 if erroneous or 0 else*/
	if (board->matrix[x][y] == 0) {
		return 0;
	}
	if (isRowErr(board,x,y) || isColErr(board,x,y) || isBlockErr(board,x,y)) {
		return 1;
	}
	else {
		return 0;
	}
}

void initStack(Board* board, DLL* stack) {
	/* iterates over the board from end to start
	 * and pushes the empty cell elements to stack*/
	int i,j;
	for (i=board->edgeSize -1; i>-1 ; i--) {
		for (j=board->edgeSize -1; j>-1 ; j--) {
			if (board->matrix[i][j] == 0) {
				/*push*/
				addFirst(stack,createNode(createStep(i,j,0,0,NULL)));
			}
		}
	}
}

int solveCell(Board* board, int x, int y) {
	/*set i as the smallest value larger than the current value*/
	int i = board->matrix[x][y] + 1;
	/*iterates over the possible values larger than the current value*/
	while (i<board->edgeSize+1) {
		/*try to set the value */
		board->matrix[x][y] = i;
		/*if the value is erroneous move on to the next value */
		if (isCellErr(board,x,y)) {
			i++;
		}
		/*else, return the value*/
		else {
			return i;
		}
	}
	/*if no legal value found, set the cell to 0 and return -1*/
	board->matrix[x][y] = 0;
	return -1;
}


int exBackTracking(Board* board){
	Board* boardCopy = NULL;
	Node* currNode = NULL;
	DLL* stack = createEmptyList();
	int solutionsNum = 0;
	boardCopy = copyBoard(board);
	/* if board allocation failed, return -999 TODO -999 value*/
	if (boardCopy == NULL) {
		return -999;
	}
	initStack(boardCopy,stack);
	currNode = stack->head;
	/*while stack isn't empty*/
	while (currNode != NULL) {
		/*if cell couldn't be filled - backtrack to previous cell*/
		if (solveCell(boardCopy, currNode->step->i, currNode->step->j) == -1) {
			currNode = currNode->prev;
		}
		/*if cell filled successfully*/
		else {
			/*if currNode is the last node then the board is successfuly filled hence increment solutionsNum*/
			if (currNode == stack->tail) {
				solutionsNum++;
			}
			/*if there are more cells to fill go to the next one*/
			else {
				currNode = currNode->next;
			}
		}
	}
	freeList(stack);
	destroyBoard(boardCopy);
	return solutionsNum;
}


/*undo == 1 if last command was undo
 *undo == 0 if last command was redo*/
void printSetUndoRedo(Board* board, int undo) { /*TODO AUX DOC*/ /*TODO printSetUndoRedo fix redo vals (upside down)*/
	Node* revertedNode;
	/*if we did undo on the first move in the list*/
	if (board->currNode == NULL) {
		revertedNode = board->movesList->head;
	}
	/*else if the the command is undo*/
	else if (undo) {
		revertedNode = board->currNode->next;
	}
	/*else if the the command is redo*/
	else {
		revertedNode = board->currNode;
	}
	if (revertedNode->step->new == 0 && revertedNode->step->old == 0) {
		if (undo) {
			printf("Undo %d,%d: from %s to %s\n", revertedNode->step->j+1,
							revertedNode->step->i+1, "_", "_");
		}
		else {
			printf("Redo %d,%d: from %s to %s\n", revertedNode->step->j+1,
							revertedNode->step->i+1, "_", "_");
		}

	}
	else if (revertedNode->step->new == 0 && revertedNode->step->old != 0) {
		if (undo) {
			printf("Undo %d,%d: from %s to %d\n", revertedNode->step->j+1,
							revertedNode->step->i+1, "_", revertedNode->step->old);
		}
		else {
			printf("Redo %d,%d: from %d to %s\n", revertedNode->step->j+1,
							revertedNode->step->i+1, revertedNode->step->old, "_");
		}
	}
	else if (revertedNode->step->new != 0 && revertedNode->step->old == 0) {
		if (undo) {
			printf("Undo %d,%d: from %d to %s\n", revertedNode->step->j+1,
							revertedNode->step->i+1, revertedNode->step->new, "_");
		}
		else {
			printf("Redo %d,%d: from %s to %d\n", revertedNode->step->j+1,
							revertedNode->step->i+1, "_", revertedNode->step->new);
		}

	}
	else {
		if (undo) {
			printf("Undo %d,%d: from %d to %d\n", revertedNode->step->j+1,
							revertedNode->step->i+1, revertedNode->step->new, revertedNode->step->old);
		}
		else {
			printf("Redo %d,%d: from %d to %d\n", revertedNode->step->j+1,
							revertedNode->step->i+1, revertedNode->step->old, revertedNode->step->new);
		}
	}
}

/*undo == 1 if last command was undo
 *undo == 0 if last command was redo*/
void printAutofillUndoRedo(Board* board, int undo) { /*TODO AUX DOC*/
	Node* innerNode;
	if (undo) {
		if (board->currNode == NULL) {
			innerNode = board->movesList->head->step->list->tail;
		}
		else {
			innerNode = board->currNode->next->step->list->tail;
		}
	}
	else {
		innerNode = board->currNode->step->list->head;
	}
	while (innerNode != NULL) {
		if (innerNode->step->new == 0 && innerNode->step->old == 0) {
			if (undo) {
				printf("Undo %d,%d: from %s to %s\n", innerNode->step->j+1,
									innerNode->step->i+1, "_", "_");
			}
			else {
				printf("Redo %d,%d: from %s to %s\n", innerNode->step->j+1,
									innerNode->step->i+1, "_", "_");
			}
		}
		else if (innerNode->step->new == 0 && innerNode->step->old != 0) {
			if (undo) {
				printf("Undo %d,%d: from %s to %d\n", innerNode->step->j+1,
						innerNode->step->i+1, "_", innerNode->step->old);
			}
			else {
				printf("Redo %d,%d: from %s to %d\n", innerNode->step->j+1,
						innerNode->step->i+1, "_", innerNode->step->old);
			}
		}
		else if (innerNode->step->new != 0 && innerNode->step->old == 0) {
			if (undo) {
				printf("Undo %d,%d: from %d to %s\n", innerNode->step->j+1,
						innerNode->step->i+1, innerNode->step->new, "_");
			}
			else {
				printf("Redo %d,%d: from %d to %s\n", innerNode->step->j+1,
						innerNode->step->i+1, innerNode->step->new, "_");
			}

		}
		else {
			if (undo) {
				printf("Undo %d,%d: from %d to %d\n", innerNode->step->j+1,
						innerNode->step->i+1, innerNode->step->new, innerNode->step->old);
			}
			else {
				printf("Redo %d,%d: from %d to %d\n", innerNode->step->j+1,
						innerNode->step->i+1, innerNode->step->new, innerNode->step->old);
			}
		}
		if (undo) {
			innerNode = innerNode->prev;
		}
		else {
			innerNode = innerNode->next;
		}
	}
} /*TODO fix auxillary function file typo (auxiliary or smth)*/

void loadToBoard(FILE* fptr, Board* board) { /*TODO AUX DOC*/
	int intFromFile,i,j, inputRead;
	char mychar;
	int oldEdgeSize = board->edgeSize;

	/*resets doubly-linked list*/
	clearList(board->movesList);

	/*reset currNode*/
	board->currNode = NULL;

	/*loads n, m into board*/
	inputRead = fscanf (fptr, "%d", &intFromFile);
	board->blockLength=intFromFile;
	assert(inputRead > 0); /*TODO ZZZ DEBUG ZZZ*/

	inputRead = fscanf (fptr, "%d", &intFromFile);
	board->blockHeight=intFromFile;
	assert(inputRead > 0); /*TODO ZZZ DEBUG ZZZ*/

	/*changes edgeSize according to n,m*/
	board->edgeSize=board->blockHeight*board->blockLength;

	/*initialize board->matrix and board->isFixed*/
	initMatrix(board,1,oldEdgeSize);
	initMatrix(board,0,oldEdgeSize);

	/*loads cell values into board*/
	for (i=0; i< board->edgeSize; i++) {
		for (j=0; j<board->edgeSize; j++) {

			inputRead = fscanf (fptr, "%d%c", &intFromFile, &mychar);
			board->matrix[i][j] = intFromFile;
			if (mychar == '.') {
				board->isFixed[i][j] = 1;
			}
			assert(inputRead > 0);
		}
	}
	fclose (fptr);
}
/*TODO When writing main function add print-board when implementing "new puzzle"*/


void fillXRandomCells(Board* board, int x) { /*TODO AUX DOC*/
	int cnt, row, col, val;
	cnt = 0;
	while (cnt<x) {
		row = rand() % (board->edgeSize);
		col = rand() % (board->edgeSize);
		val = rand() % (board->edgeSize);
		if (board->matrix[row][col] == 0) {
			board->matrix[row][col] = val+1;
			if (isCellErr(board,row,col)) {
				board->matrix[row][col] = 0;
			}
			else {
				cnt++;
			}
		}
	}
}

int ILPSolver(Board* board) { /*TODO after ILP*/
	return board->blockHeight;
}


void eraseAllButYRandomCells(Board* board, int y) { /*TODO AUX DOC*/
	int cnt, row, col;
	cnt = 0;
	while (cnt<(board->edgeSize*board->edgeSize-y)) {
		row = rand() % (board->edgeSize);
		col = rand() % (board->edgeSize);
		if (board->matrix[row][col] != 0) {
			board->matrix[row][col] = 0;
			cnt++;
		}
	}
}

void saveToFile(FILE* fptr,Board* board) {
	int i,j;
	/*first line of the file contains of the block length and block height*/
	fprintf(fptr,"%d %d\n",board->blockLength,board->blockHeight);
	/*iterate over the rows*/
	for (i=0 ; i<board->edgeSize ; i++) {
		/*iterate over the cells in row i*/
		for (j=0 ; j<board->edgeSize ; j++) {
			/*if this is the end of a row write the cell with a line drop */
			if (j == board->edgeSize-1) {
				/*if in edit mode or cell is fixed mark cell as fixed*/
				if ((board->mode == 2 && board->matrix[i][j] != 0) || board->isFixed[i][j]) {
					fprintf(fptr,"%d.\n",board->matrix[i][j]);
				}
				else {
					fprintf(fptr,"%d\n",board->matrix[i][j]);
				}
			}
			/*else, write the cell without a line drop*/
			else {
				/*if in edit mode or cell is fixed mark cell as fixed*/
				if (board->mode == 2 || board->isFixed[i][j]) {
					fprintf(fptr,"%d. ",board->matrix[i][j]);
				}
				else {
					fprintf(fptr,"%d ",board->matrix[i][j]);
				}
			}
		}
	}
	fclose(fptr);
}


/*
 *
 * Called by:
 * autofill(Board* board) from module game_logic.c
 *
 * Input:
 * Board and cell coordinates - x for the column and y for the row
 *
 * Output:
 * 0 - if the cell has more than one possible values
 * the only possible value - otherwise
 *
 * Remark:
 * Output > 0 iff the cell has one possible solution
 *
 * */
int cellHasOnePossibleValue(Board* board, int x, int y) { /*TODO AUX DOC*/
	int i,valIsValid;
	int val = 0;
	int singleSolution = 0;

	/*if cell is already filled, return -1*/
	if (board->matrix[x][y] != 0) {
		return -1;
	}

	/*iterate over the possible values*/
	for (i=1 ; i<board->edgeSize+1 ; i++) {
		/*check if value i is valid*/
		board->matrix[x][y] = i;
		valIsValid = !isCellErr(board,x,y);
		board->matrix[x][y] = 0;
		/*if it is valid and is the first solution found, save it to val
		 * if it is valid and it's a second solution return -1 (more than one solution)
		 * else try the next value*/
		if (valIsValid) {
			if (singleSolution) {
				return -1;
			}
			else {
				val = i;
				singleSolution = 1;
			}
		}
	}
	/* if we got here there are 1 or 0 possible values
	 * if there are 0 possible values return -1
	 * else, return the value*/
	if (val == 0) {
		return -1;
	}
	return val;
}
