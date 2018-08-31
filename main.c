/* Copyright 2018, Gurobi Optimization, LLC */
/*
  Sudoku example.

  The Sudoku board is a 9x9 grid, which is further divided into a 3x3 grid
  of 3x3 grids.  Each cell in the grid must take a value from 0 to 9.
  No two grid cells in the same row, column, or 3x3 subgrid may take the
  same value.

  In the MIP formulation, binary variables x[i,j,v] indicate whether
  cell <i,j> takes value 'v'.  The constraints are as follows:
    1. Each cell must take exactly one value (sum_v x[i,j,v] = 1)
    2. Each value is used exactly once per row (sum_i x[i,j,v] = 1)
    3. Each value is used exactly once per column (sum_j x[i,j,v] = 1)
    4. Each value is used exactly once per 3x3 subgrid (sum_grid x[i,j,v] = 1)

  Input datasets for this example can be found in examples/data/sudoku*.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sudoku_board.h"
#include "auxiliary_functions.h"
#include "gurobi_c.h"
#include "ilp_solver.h"

#define SUBDIM  3
#define DIM    (SUBDIM*SUBDIM)

int loadAndSolve(Board* board, char* path) {
	int suc = 0;
	FILE* fptr = fopen(path,"r");
	printf("1\n");
	if (fptr == NULL) {

		printf("couldn't load file\n");
		fclose(fptr);

		return 0;
	}
	loadToBoard(fptr, board); /*note that loadToBoard frees fptr, so don't free it again!*/
	printBoard(board);
	suc = solver(board,0);
	if (suc == -1) {
		printf("no sol found\n");
	}
	printBoard(board);
	suc = solver(board,1);
	if (suc == -1) {
			printf("no sol found\n");
	}
	printBoard(board);

	return suc;
}

int main(){
	/*test1 - 2x2 block. one cells left*/
	char* path = "oneleft.txt";
	/*test2 - 2x1 block. two cells left*/
	char* path21 = "21_2left.txt";
	/*test3 - 2x2 block. unsolvable*/
	char* pathno =  "notsolv.txt";

	Board* board = createBoard(2, 2);
	printf("2x2\n");
	loadAndSolve(board, path);
	loadAndSolve(board, path21);
	loadAndSolve(board, pathno);
	destroyBoard(board);
	printf("reached end\n");
	return 0;
}
