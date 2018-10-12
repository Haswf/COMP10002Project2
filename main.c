/* Solution to comp10002 Assignment 2, 2018 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Shuyang Fan 988301
   Dated:     12/10/2018

*/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_VERTICES 52
#define ASCII_CONVERTOR_LOWERCASE 97 /* subtract this number to get index for */
#define ASCII_CONVERTOR_UPPERCASE 39
#define UNVISITED 1
#define VISITED 0
#define FOUND 1
#define NOT_FOUND 0
#define PRINT_EDGE_LIMIT 12
#define FIRST_SIX_EDGES 6
#define FIRST_TEN_LINE 10
#define LAST_LINE -1

/* type definitions ----------------------------------------------------------*/
typedef struct Node Node_t;

typedef struct {
    char startVertex;
    char endVertex;
    int value; /* scenic value of edge*/
    int ID; /* each edge will be given a unique ID */
    int status; /* visited or unvisited */
} incident_t;

typedef incident_t data_t;
struct Node {
    data_t data;
    Node_t *next;
};
/* end type definitions ------------------------------------------------------*/

/* function prototypes -------------------------------------------------------*/

/* list operations */
void append(Node_t **head, data_t new_data);
void appendList(Node_t **lst1, Node_t **lst2);
void insertListBefore(Node_t **circuit, Node_t **trail, Node_t **joint);
void deleteList(Node_t **head);
int countList(Node_t **head);
void freeAdjacencyList(Node_t **AdjacencyList);
Node_t *searchList(Node_t **head, void *key, int(*cmp)(void *, void *));
Node_t* getTail(Node_t* head);

/* stage 0 helper functions */
char readStart(char *str);
incident_t incidentPacker(char startVertex, char endVertex, int value, int ID);
int countVertices(Node_t **adjacencyList);
void countDegree(Node_t **, int *, int *);
void checkType(int oddDegree, int evenDegree);
int readMultigraph(Node_t **adjacencyList, char routeStart);
int vertex2Index(char vertex);
void updateExtremeEdge(int edgeCount, int curr_value, int *max, int *min);
void sortAdjacencyList(Node_t** adjacencyList);

/* quicksort function */
void quickSort(struct Node **headRef, int (*cmp)(void *, void *));
Node_t *quickSortRecur(Node_t * head, Node_t * end, int (*cmp)(void *, void *));
int cmpIncident(void *data1, void *data2);
Node_t* partition(Node_t * head, Node_t * end,
                  Node_t * *newHead, Node_t * *newEnd, int (*cmp)(void *,
                                                                  void *));
/* stage 1 and 2 helper functions */
int hasUnvisitedEdge(Node_t **head);
char appendIncident(Node_t **adjacencyList, Node_t **trail, char nextVertex);
int computeScenicValue(Node_t **circuit);
void constructInitialCircuit(Node_t** adjacencyList, Node_t** circuit, char routeStart);
Node_t *constructTrail(Node_t **adjacencyList, char routeStart);
Node_t* selectVertex(Node_t **adjacencyList, Node_t **circuit);
Node_t *selectEdge(Node_t **head);
int updateStatus(Node_t **adjacencyList, Node_t *min);
void cloneAdjacencyList(Node_t **dest, Node_t **src);
Node_t* extendOneVertex(Node_t** adjacencyList, Node_t** circuit, Node_t* vertex);
void resetStatus(Node_t **adjacencyList);
void markAsvisited(Node_t **adjacencyList, char vertex, int ID);
Node_t* extendEndVertex(Node_t** adjacencyList, Node_t** circuit);
void updateAdjacencyList(Node_t **adjacencyList, Node_t** circuit);
int findBestExtension(Node_t **adjacencyList, Node_t** circuit);
void removeSandbox(Node_t** circuitModified ,Node_t** circuitCopy, Node_t** adjacencyListCopy);
void resetSandbox(Node_t** constCircuit, Node_t** circuitCopy,
                  Node_t** adjacencyList, Node_t** adjacencyListCopy);
/* print functions */
void printOutputHeader(int stage_no);
void printOutput(Node_t **circuit, int *lineCount, char* stage);
void printRestrictedCircuit(Node_t **circuit);

/* Search helper functions */
int searchID(void *node, void *key);
int searchStartVertex(void *node, void *key);

/* driver functions */
int doStage0(Node_t **adjacencyList, char routeStart);
void doStage1(Node_t **adjacencyList, char routeStart, int edgeCount);
void doStage2(Node_t **adjacencyList, char routeStart, int edgeCount);
/* end function prototypes ---------------------------------------------------*/

char readStart(char *str) {
    /* read starting point from agrv if it exists.*/
    assert(str);
    return str[0];
}

int vertex2Index(char vertex) {
    /* Compute index of a vertex in adjacencyList, returns the index */
    if (isupper(vertex)) {
        return (int) vertex - ASCII_CONVERTOR_UPPERCASE;
    } else {
        return (int) vertex - ASCII_CONVERTOR_LOWERCASE;
    }
}

int cmpIncident(void *data1, void *data2) {
    /* cmpIncident compares two incident. */
    incident_t *incident1 = data1;
    incident_t *incident2 = data2;

    /* compute difference in value */
    int diff = incident1->value - incident2->value;
    if (diff){
        return diff;
    }
    /* if they have identical values,
     * compare which vertex does the incident leads to */
    else
        return incident1->endVertex - incident2->endVertex;
}

void quickSort(Node_t **headRef, int (*cmp)(void *, void *)) {
    /* quickSort is driver function to quicksort a linked list.
     * It takes a linked list and a function pointer to compare data.*/
    (*headRef) = quickSortRecur(*headRef, getTail(*headRef), cmp);
}

Node_t *quickSortRecur(Node_t * head, Node_t * end, int (*cmp)(void *, void *)) {
    /* quickSortRecur sorts a linked list with quicksort This function is
     * a modified implementation of quicksort on linked list written by Bala.
     * All credit goes to him. The original code can be obtained from
     * https://www.geeksforgeeks.org/quicksort-on-singly-linked-list.
     * I added another parameter to pass a compare function pointer.
     * So I can compare other data type other than int*/
    /* base case */
    if (!head || head == end)
        return head;
    Node_t *newHead = NULL, *newEnd = NULL;
    /* Partition the list, newHead and newEnd will be updated by the
     * partition function */
    Node_t *pivot = partition(head, end, &newHead, &newEnd, cmp);

    /* If pivot is the smallest element - no need to recur for the left part. */
    if (newHead != pivot)
    {
        /* Set the node before the pivot node as NULL */
        Node_t *tmp = newHead;
        while (tmp->next != pivot)
            tmp = tmp->next;
        tmp->next = NULL;

        /* Recur for the list before pivot */
        newHead = quickSortRecur(newHead, tmp, cmp);

        /* Change next of last node of the left half to pivot */
        tmp = getTail(newHead);
        tmp->next = pivot;
    }

    /* Recur for the list after the pivot element */
    pivot->next = quickSortRecur(pivot->next, newEnd, cmp);
    return newHead;
}

Node_t* partition(Node_t * head, Node_t * end,
        Node_t * *newHead, Node_t * *newEnd, int (*cmp)(void *, void *)) {
    /* This function is a modified implementation of quicksort on linked list written by Bala. All credit goes to him.
     * The original code can be obtained from https://www.geeksforgeeks.org/quicksort-on-singly-linked-list.
     * I added another parameter to pass a compare function pointer. So I can compare other data type other than int*/

    /* select last node as pivot */
    Node_t *pivot = end;
    Node_t *prev = NULL, *cur = head, *tail = pivot;

    /* During partition, both the head and end of the list might change
     * which is updated in the newHead and newEnd variables */
    while (cur != pivot)
    {
        /* if current node is smaller than pivot */
        if (cmp(&cur->data,&pivot->data) < 0)
        {
            /* First node that has a value less than the pivot
             * becomes the new head */
            if ((*newHead) == NULL)
                (*newHead) = cur;

            prev = cur;
            cur = cur->next;
        }
        /* If cur node is greater than pivot, */
        else
        {
            /* Move cur node to next of tail, and change tail */
            if (prev)
                prev->next = cur->next;
            Node_t *tmp = cur->next;
            cur->next = NULL;
            tail->next = cur;
            tail = cur;
            cur = tmp;
        }
    }

    /* If the pivot data is the smallest element in the current list,
     * pivot becomes the head */
    if ((*newHead) == NULL)
        (*newHead) = pivot;

    /* Update newEnd to the current last node */
    (*newEnd) = tail;

    /* Return the pivot node */
    return pivot;
}

Node_t* getTail(Node_t* head) {
    /* getTail returns the last node in a linked list */
    while (head != NULL && head->next != NULL)
        head = head->next;
    return head;
}

void resetStatus(Node_t **adjacencyList) {
    /* resetStatus changes status of all edges in adjacencyList to unvisited */
    int i;
    /* traverse to reset status */
    for (i = 0; i < MAX_VERTICES; i++) {
        if (adjacencyList[i] != NULL) {
            Node_t *current = adjacencyList[i];
            while (current != NULL) {
                current->data.status = UNVISITED;
                current = current->next;
            }
        }
    }
}

Node_t *cloneList(Node_t *list) {
    /* cloneList make a deep copy of a list
     * It takes list head and returns pointer
     * to the new list cloned*/
    if (list == NULL)
        return NULL;
    Node_t *new = (Node_t *) malloc(sizeof(Node_t));
    new->data = list->data;
    new->next = cloneList(list->next);
    return new;
}

void cloneAdjacencyList(Node_t **dest, Node_t **src) {
    /* cloneAdjacecyList makes a deep copy of src to dest */
    int i;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (src[i] != NULL) {
            dest[i] = cloneList(src[i]);
        }
    }
}

void updateExtremeEdge(int edgeCount, int curr_value, int *max, int *min) {
    /* updateExtremeEdge() keeps track of minimum value and maximum value */
    /* Initialize minVal, maxVal with the first incident value */
    if (!edgeCount) {
        *max = *min = curr_value;
    }
        /* otherwise, update maxVal and minVal if necessary */
    else {
        if (curr_value > *max) {
            *max = curr_value;
        }
        if (curr_value < *min) {
            *min = curr_value;
        }
    }
}

int readMultigraph(Node_t **adjacencyList, char routeStart) {
    /* readMultigraph constructs an adjacencyList to store a multigraph
     * It takes an empty adjacencyList and a starting point, returns the
     * number of edges read */
    char start, end;
    /* initialise multigraph properties */
    int value = 0;
    int edgesCount, minVal, maxVal, sum, oddDegree, evenDegree;
    edgesCount = minVal = maxVal = sum = oddDegree = evenDegree = 0;

    while (scanf("%c %c %d\n", &start, &end, &value) == 3) {
        /* Call vertex2index to convert end from char to corresponding int*/
        int indexStart = vertex2Index(start);
        /* Append new incident to the list */
        append(&adjacencyList[indexStart], incidentPacker(start, end, value,
                                                          edgesCount));
        /* Call vertex2index to convert end from char to corresponding int*/
        int indexEnd = vertex2Index(end);
        /* Append new incident to the list */
        append(&adjacencyList[indexEnd], incidentPacker(end, start, value,
                                                        edgesCount));
        updateExtremeEdge(edgesCount, value, &maxVal, &minVal);
        edgesCount++;
        sum += value;
    }
    countDegree(adjacencyList, &oddDegree, &evenDegree);
    printf("S0: Map is composed of %d vertices and %d edges\n",
           countVertices(adjacencyList), edgesCount);
    printf("S0: Min. edge value: %d\n", minVal);
    printf("S0: Max. edge value: %d\n", maxVal);
    printf("S0: Total value of edges: %d\n", sum);
    printf("S0: Route starts at \"%c\"\n", routeStart);
    printf("S0: Number of vertices with odd degree: %d\n", oddDegree);
    printf("S0: Number of vertices with even degree: %d\n", evenDegree);
    checkType(oddDegree, evenDegree);
    return edgesCount;
}

incident_t incidentPacker(char startVertex, char endVertex, int value, int ID) {
    /* incidentPacker packs input into a incident_t structure */
    incident_t curr_incident;
    curr_incident.startVertex = startVertex;
    curr_incident.endVertex = endVertex;
    curr_incident.value = value;
    curr_incident.ID = ID;
    curr_incident.status = UNVISITED;
    return curr_incident;
}

void checkType(int oddDegree, int evenDegree) {
    /* Check if a multigraph is Eulerian or Traversable. If the
     * multigraph is not Eulerian it will terminates the program.
     * It takes the number of vertices of odd degree and the number of
     * vertices of even degree.*/
    if (!oddDegree && evenDegree) {
        printf("S0: Multigraph is Eulerian\n");
        return;
    } else if (oddDegree == 2) {
        printf("S0: Multigraph is traversable\n");
    }
    exit(EXIT_FAILURE);
}

int countVertices(Node_t **adjacencyList) {
    /* countVertices returns the number of vertices
     * in a adjacencylise. It takes an adjacencyList */
    int count = 0;
    int i;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (adjacencyList[i] != NULL) {
            count++;
        }
    }
    return count;
}

void countDegree(Node_t **adjacencyList, int *oddDegreeCount, int *
evenDegreeCount) {
    /* Count the number of vertices of odd degree and the number of
     * vertices of even degree in an adjacencyList. The results will be updated
     * to oddDegreeCount, evenDegreeCount passed in as pointers */
    int i, degreeCount;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (adjacencyList[i] != NULL) {
            degreeCount = countList(&adjacencyList[i]);
            if (degreeCount % 2) {
                (*oddDegreeCount)++;
            } else {
                (*evenDegreeCount)++;
            }
        }
    }
}

void freeAdjacencyList(Node_t **AdjacencyList) {
    /* Free every node in an adjacencyList */
    int i;
    /* free every non-empty linked lists in AdjacencyList*/
    for (i = 0; i < MAX_VERTICES; i++) {
        deleteList(&AdjacencyList[i]);
    }
}

void append(Node_t **head, data_t new_data) {
    /* append appends an data_t structure to the end of a linked list */
    Node_t *new = (Node_t *) malloc(sizeof(Node_t));
    /* assert malloc is successful */
    assert(new);
    /* pointer to the last node */
    Node_t *last = *head;
    /* put in data */
    new->data = new_data;
    /* make next of the new node as NULL since this will be the last node*/
    new->next = NULL;

    /* if the Linked List is empty, then make the new node as head */
    if (*head == NULL) {
        *head = new;
        return;
    }
    /* find the last node */
    while (last->next != NULL)
        last = last->next;
    /* change the next of last node to new */
    last->next = new;
}

void printOutput(Node_t **circuit, int *lineCount,
                 char* stage) {
    /* printOutput prints out stage indicator and circuit selectively
     * to meet requirements in the specification */
    assert(circuit != NULL && *circuit != NULL);
    /* static int prevLine keeps track of last line printed */
    static int prevLineCount = 0;
    if ((*lineCount <= FIRST_TEN_LINE || !(*lineCount % 5))  &&
    /* print output if the line hasn't being printed or it's the first line */
            (prevLineCount!= *lineCount || *lineCount == 1 )) {
        /* Call printRestrictedCircuit to print formatted output */
        printf("%s: ", stage);
        printRestrictedCircuit(circuit);
    }
    prevLineCount = *lineCount;
    /* increment of lineCount by 1 */
    (*lineCount)++;
}

void printRestrictedCircuit(Node_t **circuit) {
    /* printRestrictedCircuit prints a circuit with restriction*/
    assert (circuit != NULL && *circuit != NULL);
    Node_t *curr = *circuit;
    /* print start vertex of the route */
    printf("%c", curr->data.startVertex);
    /* count how many edge are visited in the circuit */
    int edgeCount = countList(circuit);
    int printCount = 0;
    while (curr != NULL) {
        if (edgeCount > PRINT_EDGE_LIMIT) {
            if (printCount < FIRST_SIX_EDGES) {
                printf("-%d->%c", curr->data.value, curr->data.endVertex);
            } else if (printCount == FIRST_SIX_EDGES) {
                printf("...");
            } else if (printCount == edgeCount - FIRST_SIX_EDGES) {
                printf("%c", curr->data.startVertex);
                printf("-%d->%c", curr->data.value, curr->data.endVertex);
            } else if (printCount > (edgeCount - FIRST_SIX_EDGES)) {
                printf("-%d->%c", curr->data.value, curr->data.endVertex);
            }
            printCount++;
        }
        /* if fewer than 12 edge are given, print all of them */
        else if (edgeCount <= PRINT_EDGE_LIMIT) {
            printf("-%d->%c", curr->data.value, curr->data.endVertex);
        }
        curr = curr->next;
    }
    printf("\n");
}

Node_t* selectVertex(Node_t **adjacencyList, Node_t **circuit) {
    assert(adjacencyList != NULL);
    assert(circuit != NULL && *circuit != NULL);
    Node_t *currIncident = *circuit;
    /* traverse to find a vertex in circuit with unvisited edge */
    while (currIncident != NULL) {
        char start = currIncident->data.startVertex;
        int startIndex = vertex2Index(start);
        Node_t* head = adjacencyList[startIndex];
        if (hasUnvisitedEdge(&head)) {
            return head;
        }
        currIncident = currIncident->next;
    }
    return NULL;
}

int hasUnvisitedEdge(Node_t **head) {
    /* Assert head and *head are valid pointer */
    assert(head != NULL && *head != NULL);
    Node_t *curr = *head;
    /* traverse list till unvisited edge found */
    while (curr != NULL) {
        if (curr->data.status) {
            return FOUND;
        }
        curr = curr->next;
    }
    return NOT_FOUND;
}

Node_t *searchList(Node_t **head, void *key, int(*cmp)(void *, void *)) {
    /* Search linked list with cmp function */
    /* Assert the given head is valid */
    assert(head != NULL && *head != NULL);
    Node_t *curr = *head;

    /* traverse to find a node which satisfies condition */
    while (curr != NULL) {
        if (cmp(curr, key)) {
            return curr;
        }
        curr = curr->next;
    }
    /* return NULL if incident satisfied condition was not presented */
    return NULL;
}

int searchID(void *node, void *key) {
    /* searchID search for a node with given ID,
     * return non-zero number if match, returns zero
     * if not match */
    Node_t *curr = node;
    int *edgeID = key;
    if (curr->data.ID == *edgeID) {
        return FOUND;
    }
    return NOT_FOUND;
}

int searchStartVertex(void *node, void *key) {
    /* searchStartVertex search for a node with given startVertex,
     * return non-zero number if found, returns zero
     * if not found */
    Node_t *curr = node;
    char *startVertex = key;
    if (curr->data.startVertex == *startVertex) {
        return FOUND;
    }
    return NOT_FOUND;
}

void deleteList(struct Node **head) {
    /* deleteList deletes a linked list */
    /* return if NULL pointer is given */
    if (head == NULL || (*head) == NULL) {
        return;
    } else {
        /* dereference head to get the real head */
        Node_t *current = *head;
        Node_t *next;

        /* delete each node */
        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
        /* Set head to NULL */
        *head = NULL;
    }
}

int countList(Node_t **head) {
    /* countList returns the number of nodes in a linked list. */
    assert(head != NULL && *head != NULL);
    int count = 0;
    /* Initialize current to be head */
    Node_t *current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void printOutputHeader(int stage_no) {
    /* The print_output_header(int stage_no) prints stage header. */
    printf("Stage %d Output\n--------------\n", stage_no);
}

Node_t *selectEdge(Node_t **head) {
    /* selectEdge returns a pointer to a node
     * which contains edge with the minumum value */
    /* Assert real head is not NULL */
    assert(head != NULL && (*head) != NULL);
    Node_t *current = *head;
    Node_t *edge = NULL;

    while (current != NULL) {
        if (current->data.status) {
            edge = current;
            break;
        }
        current = current->next;
    }
    assert(edge != NULL);
    return edge;
}

void markAsvisited(Node_t **adjacencyList, char vertex, int ID){
    Node_t* curr = adjacencyList[vertex2Index(vertex)];
    while (curr!=NULL){
        /* if edge with specified ID was found */
        if (curr->data.ID == ID){
            curr->data.status = VISITED;
            assert(curr->data.status == VISITED);
        }
        curr = curr->next;
    }
}
int updateStatus(Node_t **adjacencyList, Node_t *min) {
    markAsvisited(adjacencyList, min->data.startVertex, min->data.ID);
    markAsvisited(adjacencyList, min->data.endVertex, min->data.ID);
    return 0;
}

char appendIncident(Node_t **adjacencyList, Node_t **trail, char nextVertex) {
    assert(trail != NULL);
    int nextIndex = vertex2Index(nextVertex);
    /*  find edge with minimum value for current vertex */
    Node_t *edge = selectEdge(&adjacencyList[nextIndex]);
    /* change status of a pair of incidents to visited */
    updateStatus(adjacencyList, edge);
    nextVertex = edge->data.endVertex;
    /* compute index of the next vertex */
    append(trail, edge->data);
    return nextVertex;
}

Node_t *constructTrail(Node_t **adjacencyList, char routeStart) {
    char nextVertex = routeStart;
    Node_t *trail = NULL;
    int circuitCompleted = 0;
    /* keep moving till reached the start vertex, the circuit is completed */
    while (!circuitCompleted) {
        nextVertex = appendIncident(adjacencyList, &trail, nextVertex);
        /* if get back to starting point, circuit is completed */
        if (nextVertex == routeStart) {
            circuitCompleted++;
        }
    }
    return trail;
}

void sortAdjacencyList(Node_t** adjacencyList){
    /* sortAdjacencyList sorts each linked list
     * in adjacencyList with quicksort*/
    int i;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (adjacencyList[i] != NULL) {
            quickSort(&adjacencyList[i], &cmpIncident);
        }
    }
}
int doStage0(Node_t **adjacencyList, char routeStart) {
    printOutputHeader(0);
    int edgeCount = readMultigraph(adjacencyList, routeStart);
    sortAdjacencyList(adjacencyList);
    return edgeCount;
}

void doStage1(Node_t **adjacencyList, char routeStart, int edgeCount) {
    printOutputHeader(1);
    int lineCount = 1;
    /* Linked list that records incident travelled */
    Node_t *circuit = NULL;

    /* Construct initial circuit */
    constructInitialCircuit(adjacencyList, &circuit, routeStart);
    printOutput(&circuit, &lineCount, "S1");
    int edgeVisited = countList(&circuit);

    /* keep constructing new circuit until all edge have been visited */
    while (edgeVisited != edgeCount) {
        Node_t *curr = selectVertex(adjacencyList, &circuit);
        char nextVertex = curr->data.startVertex;
        Node_t *trail = constructTrail(adjacencyList, nextVertex);
        /* find where to join the new circuit */
        Node_t *joint = searchList(&circuit, &nextVertex, &searchStartVertex);
        insertListBefore(&circuit, &trail, &joint);
        /* Update the number of edges visited and compare it with edgeCount */
        if ((edgeVisited = countList(&circuit)) == edgeCount){
            lineCount = LAST_LINE;
        }
        printOutput(&circuit, &lineCount, "S1");
    }
    printf("S1: Scenic route value is %d\n", computeScenicValue(&circuit));
    /* clean up */
    deleteList(&circuit);
    circuit = NULL;
}

void constructInitialCircuit(Node_t** adjacencyList, Node_t** circuit, char
routeStart){
    /* constructInitialCircuit constructs a initial circuit with starting point
     * specified. It takes an adjacencylist, a linked list to save the
     * circuit and a char that indicates where to start. */
    Node_t *joint = NULL;
    /* Construct initial circuit */
    Node_t *trail = constructTrail(adjacencyList, routeStart);
    insertListBefore(circuit, &trail, &joint);
}

void updateAdjacencyList(Node_t **adjacencyList, Node_t** circuit){
    /* Mark all edges in circuit as visited in adjacencyList */
    Node_t* incident = *circuit;
    while (incident!=NULL){
        updateStatus(adjacencyList, incident);
        incident = incident->next;
    }
}

void changeGlobalCircuit(Node_t** circuitCopy, Node_t** circuit, int* max) {
    /* changeGlobalCircuit compares two circuits. If the new circuit contributes
     * to a higher overall scenic value, replace the global circuit with the
     * new one */
    int curr = computeScenicValue(circuitCopy);
    if (curr > *max) {
        *max = curr;
        deleteList(circuit);
        *circuit = cloneList(*circuitCopy);
    }
}

void resetSandbox(Node_t** constCircuit, Node_t** circuitCopy,
        Node_t** adjacencyList, Node_t** adjacencyListCopy){
    /* reset adjacencyListCopy and circuitCopy for other functions to mess
     * around */
    cloneAdjacencyList(adjacencyListCopy, adjacencyList);
    *circuitCopy = cloneList(*constCircuit);
}

void removeSandbox(Node_t** circuitModified ,Node_t** circuitCopy,
        Node_t** adjacencyListCopy){
    /* clean up adjacencyListCopy and circuitCopy */
    if (*circuitModified){
        deleteList(circuitModified);
    }
    else
        deleteList(circuitCopy);
    freeAdjacencyList(adjacencyListCopy);
    /* set pointers to NULL */
    *circuitCopy = NULL;
    *circuitModified = NULL;
    *adjacencyListCopy = NULL;
}
int findBestExtension(Node_t **adjacencyList, Node_t** circuit) {
    /* findBestExtension literally finds the best extension which
     * contributes to the highest scenic value. It applies change to the
     * global circuit. */
    int max = 0;

    Node_t* constCircuit = cloneList(*circuit);
    Node_t* adjacencyListCopy[MAX_VERTICES] = {NULL};
    Node_t* circuitCopy = NULL;

    /* Attempt to extend at each vertex in current circuit*/
    Node_t* currIncident = constCircuit;
    while (currIncident != NULL) {
        resetSandbox(&constCircuit, &circuitCopy, adjacencyList, adjacencyListCopy);
        Node_t* circuitCopyModified = extendOneVertex(adjacencyListCopy, &circuitCopy,
                                       currIncident);
        if (circuitCopyModified){
            changeGlobalCircuit(&circuitCopyModified, circuit, &max);
        }
        removeSandbox(&circuitCopyModified,&circuitCopy, adjacencyListCopy);
        currIncident = currIncident->next;
    }

    /* Attempt to extend at last vertex */
    resetSandbox(&constCircuit, &circuitCopy, adjacencyList, adjacencyListCopy);
    Node_t* circuitCopyModified = extendEndVertex(adjacencyListCopy, &circuitCopy);
    if (circuitCopyModified != NULL) {
        changeGlobalCircuit(&circuitCopyModified, circuit, &max);
    }
    removeSandbox(&circuitCopyModified,&circuitCopy, adjacencyListCopy);

    /* Sync changes to global adjacencyList */
    updateAdjacencyList(adjacencyList, circuit);

    /* clean up */
    deleteList(&constCircuit);
    return countList(circuit);
}

Node_t* extendEndVertex(Node_t** adjacencyList, Node_t** circuit){
    /* extendEndVertex attempts to extend current circuit at its end,
     * returns a modified linked list after extending at end */
    Node_t *lastNode = *circuit;
    /* locate the end of vertex */
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }
    char nextVertex = lastNode->data.endVertex;
    int nextIndex = vertex2Index(nextVertex);
    Node_t* trail = NULL;
    if (hasUnvisitedEdge(&adjacencyList[nextIndex])) {
        trail = constructTrail(adjacencyList, nextVertex);
        appendList(circuit, &trail);
    }
    return *circuit;
}

Node_t* extendOneVertex(Node_t** adjacencyList, Node_t** circuit, Node_t*
vertex){
    Node_t* trail = NULL;
    Node_t* joint = NULL;
    char nextVertex = vertex->data.startVertex;
    int nextIndex = vertex2Index(nextVertex);
    if (hasUnvisitedEdge(&adjacencyList[nextIndex])){
        trail = constructTrail(adjacencyList, nextVertex);
        joint = searchList(circuit, &vertex->data.ID, &searchID);
        insertListBefore(circuit, &trail, &joint);
        return *circuit;
    }
    else {
        return NULL;
    }
}

void doStage2(Node_t **adjacencyList, char routeStart, int edgeCount) {
    printOutputHeader(2);
    /* Linked list that records incident travelled */
    Node_t *circuit = NULL;
    /* initialise lintCount with 1 */
    int lineCount = 1;

    /* construct first circuit */
    constructInitialCircuit(adjacencyList, &circuit, routeStart);
    printOutput(&circuit, &lineCount, "S2");
    /* update edgeVisited */
    int edgeVisited = countList(&circuit);

    while (edgeVisited != edgeCount) {
        /* Update the number of edges visited */
        edgeVisited = findBestExtension(adjacencyList, &circuit);
        /* if last circuit reached, instruct printOutput to print it */
        if (edgeVisited == edgeCount){
            lineCount = LAST_LINE;
        }
        printOutput(&circuit, &lineCount, "S2");
    }
    printf("S2: Scenic route value is %d\n", computeScenicValue(&circuit));
    /* clean up */
    deleteList(&circuit);
    circuit = NULL;
}

void appendList(Node_t **lst1, Node_t **lst2) {
    /* append an entire linked to another linked list */
    assert(lst1 != NULL && lst2 != NULL);
    assert(*lst1 != NULL && *lst1 != NULL);
    Node_t *end = *lst1;
    /* locate the last node of list 1 */
    while (end->next != NULL) {
        end = end->next;
    }
    end->next = *lst2;
}

void insertListBefore(Node_t **circuit, Node_t **trail, Node_t **joint) {
    /* Insert an entire linked list trial before a node
     * in circuit specified by pointer joint */
    assert(circuit != NULL);
    assert(*trail != NULL && trail != NULL);
    /* if given linked list is empty */
    if (*circuit == NULL) {
        *circuit = *trail;
        return;
    }
    /* if given linked list to be inserted is empty */
    if (*trail == NULL) {
        return;
    }
    Node_t *curr = *circuit;
    Node_t *prev = NULL;
    while (curr->data.ID != (*joint)->data.ID) {
        prev = curr;
        curr = curr->next;
    }
    Node_t *trailHead = *trail;
    curr = *trail;

    Node_t* trailEnd = NULL;
    while (curr != NULL) {
        trailEnd = curr;
        curr = (curr)->next;
    }
    assert(trailEnd != NULL);
    /* if list is to be inserted at the front of circuit */
    if (prev == NULL) {
        *circuit = trailHead;
        trailEnd->next = (*joint);
    }
        /* if list is to be inserted at the middle of the circuit */
    else {
        prev->next = *trail;
        trailEnd->next = (*joint);
    }
}

int computeScenicValue(Node_t **circuit) {
    /* Compute overall scenic value of a circuit.
     * It takes a linked list contains edges visited in the circuit,
     * returns overall scenic value. */
    Node_t *curr = *circuit;
    int weight = 1;
    int value = 0;
    /* traverse circuit and add up scenic value with the given formula */
    while (curr != NULL) {
        value += (curr->data.value) * weight;
        /* increase weight by 1 */
        weight++;
        curr = curr->next;
    }
    return value;
}

int main(int argc, char *argv[]) {
    /* driver functions to perform tasks in stage 0, 1, 2 */
    Node_t *adjacencyList[MAX_VERTICES] = {NULL};
    /* read routestart from argv */
    char routeStart = readStart(argv[1]);
    /* read multigraph from stdin and save number of edge read */
    int edgeCount = doStage0(adjacencyList, routeStart);
    doStage1(adjacencyList, routeStart, edgeCount);
    /* reset status of all edges to unvisited */
    resetStatus(adjacencyList);
    doStage2(adjacencyList, routeStart, edgeCount);
    /* clean up */
    freeAdjacencyList(adjacencyList);
    return EXIT_SUCCESS;
}
/* Algorithms are fun */

