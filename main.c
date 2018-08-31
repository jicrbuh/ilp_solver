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
	FILE* fptr = fopen(path,"r");
	if (fptr == NULL) {
		printf("couldn't load file\n");
		return 0;
	}
	loadToBoard(fptr, board);
	printBoard(board);
	solver(board,0);
	printBoard(board);
	solver(board,1);
	printBoard(board);
	fclose(fptr);
	return 1;
}

int main(){
	/*test1 - 2x2. one square left
	char* path = "oneleft.txt";*/
	char* path21 = "21_3left.txt";
	FILE* fptr = fopen(path21,"r");
	Board* board = createBoard(2, 2);
	/*
	printf("2x2\n");
	loadAndSolve(board, path);
	*/
	printf("1x2\n");
	if (fptr == NULL) {
			printf("couldn't load file\n");
			return 0;
	}
	loadToBoard(fptr, board);
	printBoard(board);
	/*loadAndSolve(board, path21);
	destroyBoard(board);*/
	return 0;
}
