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
#include "gurobi_c.h"
#include "sudoku_board.h"

void createEmptyProj(int dim, int** matrix, char** names, char* vtype, double* lb, char* cursor,char* namestorage );
void decrement(int** matrix, int size);
int addCellValue(int dim,int* ind, double* val,int *errorptr, GRBmodel *model );
void addColConstr(int dim, int* ind, double* val,int *errorptr, GRBmodel *model);
void addRowConstr(int dim, int* ind, double* val,int *errorptr, GRBmodel *model);
void addBlockConstr(int dim, int blockLength, int blockHeight, int* ind, double* val,int *errorptr, GRBmodel *model);
void addConstr(int dim, int blockLength, int blockHeight, int* ind, double* val,int *errorptr, GRBmodel *model);
void freeOnlyMatrix(int** matrix, int dim);

int solver(Board* board) {
	 /*FILE     *fp    = NULL;todo delete?*/
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int       i, j, v, ig, jg, optimstatus, count,zero = 0, error = 0,dim = board->edgeSize;
	int*		ind = (int*)calloc(dim,sizeof(int));
	double*   val = (double*)calloc(dim,sizeof(double));
	double*   lb = (double*)calloc(dim*dim*dim,sizeof(double));
	char*     vtype = (char*)calloc(dim*dim*dim,sizeof(char));
	char**    names = (char**)calloc(dim*dim*dim,sizeof(char*));
	char*     namestorage= (char*)calloc(10*dim*dim*dim,sizeof(char));
	char*     cursor;
	double    objval;
	int ** 	copiedMtx;
	printf("break 11\n");

	/*allocate memory for the board's matrix*/
	copiedMtx = calloc(board->edgeSize, sizeof(int*));
	createMatrix(copiedMtx, dim);
	printBoard(board);

	/*copied the matrix from the board to copiedBoard*/
	copyMatrix(board->matrix, copiedMtx, dim);
	printf("break 111\n");

	/*decrement each  cell by 1. empty cells are noted by -1*/
	decrement(copiedMtx, dim);
	printf("break 2\n");

	/* Create an empty model, and the env */
	createEmptyProj(dim, copiedMtx, names, vtype, lb,cursor,namestorage);
	printf("break 3\n");
	error = GRBloadenv(&env, "sudoku.log");
	if (error) goto QUIT;

	/* Create new model */
	error = GRBnewmodel(env, &model, "sudoku", dim*dim*dim, NULL, lb, NULL, vtype, names);
	if (error) goto QUIT;
	printf("break 4\n");

	/* Each cell gets a value */
	addCellValue(dim, ind, val,&error, model);
	if (error) goto QUIT;
	printf("break 5\n");

	/* add constraints */
	addConstr(dim, board->blockLength, board->blockHeight, ind, val, &error, model);

	/* Optimize model */
	error = GRBoptimize(model);
	if (error) goto QUIT;

	/*print copyMtx for debug purposes*/
	for (i=0 ; i < board->edgeSize ; i++) {
		for (j=0 ; j < board->edgeSize ; j++) {
			printf("%f ", val[i*board->edgeSize + j]);
		}
	}
	/* Write model to 'sudoku.lp' */
	error = GRBwrite(model, "sudoku.lp");
	if (error) goto QUIT;

	/* Capture solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) goto QUIT;
	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) goto QUIT;

	printf("\nOptimization complete\n");
	if (optimstatus == GRB_OPTIMAL) {
		printf("Optimal objective: %.4e\n", objval);
	}
	else if (optimstatus == GRB_INF_OR_UNBD) {
		printf("Model is infeasible or unbounded\n");
	}
	else {
		printf("Optimization was stopped early\n");
		printf("\n");
	}

	printf("break 9\n");
	QUIT:

	printf("QUIT start\n");
	/* Error reporting */
	printf("before if(error)\n");
	if (error) {
		printf("ERROR: %s\n", GRBgeterrormsg(env));
		exit(1);
	}
	printf("before fclose(fp)\n");
	/*fclose(fp);*/

	/* Free model and env*/
	printf("before free\n");
	GRBfreemodel(model);
	GRBfreeenv(env);
	printf("QUIT end\n");
	/*free copiedMtx*/
	freeOnlyMatrix(copiedMtx, dim);
	return 0;
}


int createMatrix(int** matrix, int size) {
	int i;
	if (matrix == NULL) {
			return 999; /*if allocation failed, returns NULL*/
		}
	for(i=0 ;i < size; i++) {
		matrix[i] = calloc(size, sizeof(int));
	}
	printf("%d\n",size);
	return 0;

}

void decrement(int** matrix, int size){
	int i,j;
	for(i=0; i<size ; i++) {
		  for(j=0; j<size ; j++) {
			  matrix[i][j] = matrix[i][j] - 1;
			}
		}
}

void createEmptyProj(int dim, int** matrix, char** names, char* vtype, double* lb, char* cursor,char* namestorage ){
	int i,j,v;
	cursor = namestorage;
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			for (v = 0; v < dim; v++) {
				if (matrix[i][j] == v) {
					lb[i*dim*dim+j*dim+v] = 1;
				}
				else {
					lb[i*dim*dim+j*dim+v] = 0;
				}
		vtype[i*dim*dim+j*dim+v] = GRB_BINARY;

		names[i*dim*dim+j*dim+v] = cursor;
		sprintf(names[i*dim*dim+j*dim+v], "x[%d,%d,%d]", i, j, v+1);
		cursor += strlen(names[i*dim*dim+j*dim+v]) + 1;
			}
		}
	}
}

int addCellValue(int dim,int* ind, double* val,int *errorptr, GRBmodel *model ){
	/* Each cell gets a value */
	int i,j,v;
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			for (v = 0; v < dim; v++) {
				ind[v] = i*dim*dim + j*dim + v;
				val[v] = 1.0;
				}

			*errorptr = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
			if (*errorptr) {
				return 1;
			}
		}
	}
	return 0;
}

void addRowConstr(int dim,int* ind, double* val,int *errorptr, GRBmodel *model) {
	int i,j,v;
	for (v = 0; v < dim; v++) {
		for (j = 0; j < dim; j++) {
			for (i = 0; i < dim; i++) {
				ind[i] = i*dim*dim + j*dim + v;
				val[i] = 1.0;
			}

			*errorptr = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
			if (*errorptr) return;
		}
	}
}

void addColConstr(int dim, int* ind, double* val,int *errorptr, GRBmodel *model) {
	int i,j,v;
	for (v = 0; v < dim; v++) {
		for (i = 0; i < dim; i++) {
			for (j = 0; j < dim; j++) {
				ind[j] = i*dim*dim + j*dim + v;
				val[j] = 1.0;
			}

			*errorptr = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
			if (*errorptr) return;
		}
	}
}

void addBlockConstr(int dim, int blockLength, int blockHeight, int* ind, double* val,int *errorptr, GRBmodel *model) {
	int i,j,ig,jg,v, count;
	for (v = 0; v < dim; v++) {
			for (ig = 0; ig < blockLength; ig++) {
				for (jg = 0; jg < blockHeight; jg++) {
					count = 0;
					for (i = ig*blockHeight; i < (ig+1)*blockHeight; i++) {
						for (j = jg*blockLength; j < (jg+1)*blockLength; j++) {
							ind[count] = i*dim*dim + j*dim + v;
							val[count] = 1.0;
							count++;
						}
					}
					*errorptr = GRBaddconstr(model, dim, ind, val, GRB_EQUAL, 1.0, NULL);
					if (*errorptr) return;
				}
			}
		}
}

void addConstr(int dim, int blockLength, int blockHeight, int* ind, double* val,int *errorptr, GRBmodel *model){
	/* Each value must appear once in each row */
	addRowConstr(dim,ind, val,errorptr, model);
	printf("break 6\n");
	if (*errorptr) return;


	/* Each value must appear once in each column */
	addColConstr(dim,ind, val,errorptr, model);
	printf("break 7\n");
	if (*errorptr) return;


	/* Each value must appear once in each subgrid */
	addBlockConstr(dim, blockLength, blockHeight, ind, val, errorptr, model);
	printf("break 8\n");
	if (*errorptr) return;

}

void freeOnlyMatrix(int** matrix, int dim) {
	/*free copiedMtx*/
	int i;
	for (i=0 ; i< dim ; i++) {
		free(matrix[i]);
	}
	free(matrix);
}
