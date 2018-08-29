/*
 * newDLL.h
 *
 *  Created on: 6 Aug 2018
 *      Author: chen
 */

#ifndef DOUBLY_LINKED_LIST_H_
#define DOUBLY_LINKED_LIST_H_
#include "SPBufferset.h"

typedef struct Step{
	int i;
	int j;
	int old;
	int new;
	struct DLL* list;
} Step;

typedef struct Node  {
	Step* step;
	struct Node* next;
	struct Node* prev;
} Node;



typedef struct DLL {
	Node* head;
	Node* tail;
} DLL;

/*
 * Input:
 * None
 *
 * Output:
 * Empty doubly linked list */
DLL* createEmptyList();


/*
 * Input:
 * i,j - coordinates of the cell in the board, i for row, j - for column.
 * new - matrix[i][j] current value, old - matrix[i][j] as was before added the new value.
 * list - a doubly linked list.
 *
 * Output:
 * a pointer to the step described above */
Step* createStep (int i, int j, int old, int new, DLL* list);

/* Input:
 * a Step
 *
 * Output:
 * Node* that points to Step */
Node* createNode(Step *step);


/* Input:
 * pointer to a doubly linked list, step - pointer to a step to be added as the content of a new last node
 *
 * Output:
 * None
 *
 * Description:
 * adds a node to the end of list*/
void addLast(DLL* list, Node* node);

/* Input:
 * pointer to a doubly linked list, step - pointer to a step to be added as the content of a new last node
 *
 * Output:
 * None
 *
 * Description:
 * adds a node to the beginning of list*/
void addFirst (DLL* list, Node* node);

/* Input:
 * Doubly linked list
 *
 * Output:
 * None
 *
 * Description:
 * deletes the first node in list and frees all resources used by the node*/
void deleteFirst(DLL* list);

/* Input:
 * Doubly linked list
 *
 * Output:
 * None
 *
 * Description:
 * deletes the last node in list and frees all resources used by the node*/
void deleteLast(DLL* list);

/* Input:
 * list - doubly linked list, node - the node to be last after the operation
 *
 * Output:
 * None
 *
 * Description:
 * deletes all the nodes after node (not including node),
 * and frees all resources used by all the deleted nodes*/
void deleteAllNextNodes(DLL* list, Node* node);

/* Input:
 * step - a step to be printed
 *
 * Output:
 * None
 *
 * Description:
 * prints step in the following format:
 * "(i,j): old -> new\n"
 *
 * Remark: for debug perpuses only */
void printStep(Step* step);

/* Input:
 * list - a list to be printed
 *
 * Output:
 * None
 *
 * Description:
 * calls printStep(step) for all the nodes' steps in list from first to last */
void printList(DLL* list);

/* Input:
 * list - a list to be printed
 *
 * Output:
 * None
 *
 * Description:
 * calls printStep(step) for all the nodes' steps in list from last to first */
void printListFromTail(DLL* list);


void clearList(DLL* list);/*TODO DOC*/

/* Input:
 * a doubly linked list
 *
 * Output:
 * None
 *
 * Description:
 * frees all resources used by the list */
void freeList (DLL* list);

void freeNode(Node* node); /*TODO DOC*/


#endif /* DOUBLY_LINKED_LIST_H_*/
