CC = gcc
OBJS = main.o ilp_solver.o auxiliary_functions.o doubly_linked_list.o sudoku_board.o 
 
EXEC = myprog
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c sudoku_board.h auxiliary_functions.h SPBufferset.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
sudoku_board.o: sudoku_board.c auxiliary_functions.h doubly_linked_list.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
auxiliary_functions.o: sudoku_board.c auxiliary_functions.h doubly_linked_list.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
doubly_linked_list.o: doubly_linked_list.c
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
clean:
	rm -f *.o $(EXEC)
