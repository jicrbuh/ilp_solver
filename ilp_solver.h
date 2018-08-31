/*
 * solver.h
 *
 *  Created on: 3 Aug 2018
 *      Author: chen
 */

#ifndef ILP_SOLVER_H_
#define ILP_SOLVER_H_
#include "sudoku_board.h"


/*Based on the example in the GUROBI website.
 *
 * solves a sudoku board using ILP throught the gurobi library.
 *
 * Input:
 * board - a pointer to the sudoku board to be solved.
 * saveToBoard - if 1 the solution will be saved to the board, else it won't.
 *
 * Output:
 * -1 if no solution was found, 0 if a solution was found*/
int solver(Board* board, int saveToBoard);


#endif /* ILP_SOLVER_H_ */
