

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gurobi_c.h"
#include "sudoku_board.h"
#include "SPBufferset.h"

void createEmptyProj(int dim, int** matrix, char** names, char* vtype, double* lb, char* cursor,char* namestorage );
void decrement(int** matrix, int size);
int addCellValue(int dim,int* ind, double* val,int *errorptr, GRBmodel *model );
void addColConstr(int dim, int* ind, double* val,int *errorptr, GRBmodel *model);
void addRowConstr(int dim, int* ind, double* val,int *errorptr, GRBmodel *model);
void addBlockConstr(int dim, int blockLength, int blockHeight, int* ind, double* val,int *errorptr, GRBmodel *model);
void addConstr(int dim, int blockLength, int blockHeight, int* ind, double* val,int *errorptr, GRBmodel *model);
void freeOnlyMatrix(int** matrix, int dim);
void printArr(double* val, int dim); /*todo delete only this function*/
void insertSol(double* sol, int** matrix, int dim);
int findCellSol(double* sol, int** matrix, int dim, int x, int y);
void printMtxNoBoard(int** matrix, int dim);
void increment(int** matrix, int size);

/* Based on the example in the GUROBI website.
 *
 * if saveToBoard = 1, the soduko solution will be saved to the board->matrix
 *
 * returns 0 if succeeded in solving the sudoku
 * return -1 if no solution found
*/
int solver(Board* board, int saveToBoard) {
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int       i, j, v, ig, jg, optimstatus, count,zero = 0, error = 0,dim = board->edgeSize;
	int*		ind = (int*)calloc(dim,sizeof(int));
	double*   val = (double*)calloc(dim,sizeof(double));
	double*   lb = (double*)calloc(dim*dim*dim,sizeof(double)); /* variable lower bounds */
	char*     vtype = (char*)calloc(dim*dim*dim,sizeof(char));
	char**    names = (char**)calloc(dim*dim*dim,sizeof(char*));
	char*     namestorage= (char*)calloc(10*dim*dim*dim,sizeof(char));
	char*     cursor;
	double    objval;
	double* 	sol = (double*)calloc(dim*dim*dim,sizeof(double));
	int ** 	copiedMtx;

	/*allocate memory for the board's matrix*/
	copiedMtx = calloc(board->edgeSize, sizeof(int*));
	createMatrix(copiedMtx, dim);

	/*copied the matrix from the board to copiedBoard*/
	copyMatrix(board->matrix, copiedMtx, dim);

	/*decrement each  cell by 1. empty cells are noted by -1*/
	decrement(copiedMtx, dim);

	/* Create an empty model, and the env */
	createEmptyProj(dim, copiedMtx, names, vtype, lb,cursor, namestorage);
	error = GRBloadenv(&env, "sudoku.log");/*todo do we need this?*/
	if (error) goto QUIT;

	/*cancel gurobi's automatic printing*/
	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) goto QUIT;

	/* Create new model */
	error = GRBnewmodel(env, &model, "sudoku", dim*dim*dim, NULL, lb, NULL, vtype, names);
	if (error) goto QUIT;

	/* Each cell gets a value */
	addCellValue(dim, ind, val,&error, model);
	if (error) goto QUIT;

	/* add constraints */
	addConstr(dim, board->blockLength, board->blockHeight, ind, val, &error, model);
	if (error) goto QUIT;

	/* Optimize model */
	error = GRBoptimize(model);
	if (error) goto QUIT;

	/* Write model to 'sudoku.lp' */
	error = GRBwrite(model, "sudoku.lp");
	if (error) goto QUIT;

	/* Capture solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) goto QUIT;
	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) goto QUIT;

	/*printf("\nOptimization complete\n");/*todo delete*/

	if (optimstatus == GRB_OPTIMAL) {
		/*printf("Optimal objective: %.4e\n", objval);/*todo delete*/
	}

	else if (optimstatus == GRB_INF_OR_UNBD) {
		/*printf("Model is infeasible or unbounded\n");/*todo delete*/
		goto QUIT;
	}
	else {
		/*printf("Optimization was stopped early\n\n");/*todo delete*/
		goto QUIT;
	}

	/*if the optimization succeeded, save the solution to the dim*dim*dim matrix sol*/
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, dim*dim*dim, sol);
	if (error) goto QUIT;

	/*increment back copiedMtx*/
	increment(copiedMtx, dim);

	/*use the values in sol to write the solved matrix to copiedMtx*/
	insertSol(sol, copiedMtx, dim);

	/*if saveToBoard=1, copy the sudoku solution to board->matrix*/
	if (saveToBoard == 1) {
		copyMatrix(copiedMtx, board->matrix, dim);
	}

	QUIT:
	/* Error reporting */
	if (error) {
		/*printf("ERROR: %s\n", GRBgeterrormsg(env)); todo delete*/
	}

	/* Free model, env, copiedMtx*/
	GRBfreemodel(model);
	GRBfreeenv(env);
	freeOnlyMatrix(copiedMtx, dim);
	free(ind); free(lb); free(vtype); free(names); free(namestorage); free(sol); free(val);
	if (error) {
		return -1;
	}
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

void increment(int** matrix, int size){
	int i,j;
	for(i=0; i<size ; i++) {
		  for(j=0; j<size ; j++) {
			  matrix[i][j] = matrix[i][j] + 1;
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
					lb[i*dim*dim+j*dim+v] = 1; /*LB = lowerbound*/
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
	/* for example:
	 * A constraint: x + 2 y + 3 z <= 4
  	 * ind[0] = 0; ind[1] = 1; ind[2] = 2;
  	 * val[0] = 1; val[1] = 2; val[2] = 3;*/
	int i,j,v;
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			for (v = 0; v < dim; v++) {
				ind[v] = i*dim*dim + j*dim + v; /*ind[v] equals the variable's index*/
				val[v] = 1.0; /*val[v] equals the variable index*/
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

	if (*errorptr) return;

	/* Each value must appear once in each column */
	addColConstr(dim,ind, val,errorptr, model);

	if (*errorptr) return;
	/* Each value must appear once in each subgrid */
	addBlockConstr(dim, blockLength, blockHeight, ind, val, errorptr, model);

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

void printArr(double* val, int dim) {
	int i,j;
	printf("\nprinting the matrix:\n");
	for (i=0 ; i < dim ; i++) {
			for (j=0 ; j < dim ; j++) {
				if (j%dim == 0) {
					printf("\n");
				}
				printf("%.2f ", val[i*dim + j]);
			}
		}
}
/*reads the solution from sol[] into matrix*/
void insertSol(double* sol, int** matrix, int dim) {
	/*sol is a [dim*dim*dim] array. matrix is a [dim][dim] matrix*/
	int i,j, val;
	for(i=0; i<dim; i++) {
		for(j=0; j<dim; j++) {
			if(matrix[i][j] == 0) { /*if the cell is empty, find its right value in sol*/
				matrix[i][j] = findCellSol(sol,matrix, dim, i, j);
			}
		}
	}
}
/*returns cell x,y value according to the solution array sol[dim*dim*dim]
 * if can't find one, returns -1*/
int findCellSol(double* sol, int** matrix, int dim, int x, int y) {
	int i,v;
	int start =	dim*dim*x + dim*y;
	for (i=0; i<dim; i++) {
		if ((int)sol[start+i] == 1) {
			return i+1;
		}
	}
	return -1;
}

void printMtxNoBoard(int** matrix, int dim) {
	int i,j;
	printf("\n");
	for(i=0; i<dim; i++) {
			for(j=0; j<dim; j++) {
				printf("%d ", matrix[i][j]);
				if (j+1 == dim) {
					printf("\n");
				}

			}
		}
}
