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
   Dated:     30/09/2018

*/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_VERTICES 52
#define ASCII_CONVERTOR_LOWERCASE 97 /* subtract this number to get index*/
#define ASCII_CONVERTOR_UPPERCASE 39
#define UNVISITED 1
#define VISITED 0
#define FOUND 1
#define NOT_FOUND 0
#define PRINT_EDGE_LIMIT 12
#define FIRST_TEN_LINE 10
#define A_NEGATIVE_NUMBER -1
/* type definitions ----------------------------------------------------------*/
typedef struct Node Node_t;

typedef struct {
    char startVertex;
    char endVertex;
    int value;
    int ID;
    int status;
} incident_t;

typedef incident_t data_t;
struct Node {
    data_t data;
    Node_t *next;
};
/* end type definitions ------------------------------------------------------*/

/* function prototypes -------------------------------------------------------*/

// list operations
void append(Node_t **head, data_t new_data);

void insertBefore(Node_t **circuit, Node_t **walk, Node_t **joint);

void deleteList(Node_t **head);

int countList(Node_t **head);

void freeAdjacencyList(Node_t **AdjacencyList);

void mergeList(Node_t **lst1, Node_t **lst2);

incident_t incidentPacker(char startVertex, char endVertex, int value, int ID);

int countVertices(Node_t **adjacencyList);

void countDegree(Node_t **, int *, int *);

void checkType(int oddDegree, int evenDegree);

int readMultigraph(Node_t **adjacencyList, char routeStart);

/* print-related functions */
void printList(Node_t **head);

void printMultigraph(Node_t **adjacencyList);

void printOutputHeader(int stage_no);

void printIncident(incident_t incident);

void printRestrictedCircuit(Node_t **circuit);

void printOutput(Node_t **circuit, int *lineCount);

void resetStatus(Node_t **adjacencyList);

int hasUnvisitedEdge(Node_t **head);

char appendIncident(Node_t **adjacencyList, Node_t **walk, char nextVertex);

int computeScenicValue(Node_t **circuit);

char readStart(char *str);

int vertex2Index(char vertex);

void ConstructInitialCircuit(Node_t** adjacencyList, Node_t** circuit, char
routeStart);

void updateExtremeEdge(int edgeCount, int curr_value, int *max, int *min);

/* Search helper functions */
int searchID(void *node, void *key);

int searchStartVertex(void *node, void *key);

char selectVertex(Node_t **adjacencyList, Node_t **circuit);

Node_t *findMinValue(Node_t **head);

int updateStatus(Node_t **adjacencyList, Node_t *min);

Node_t *constructCircuit(Node_t **adjacencyList, char routeStart);

void do_stage0(Node_t **adjacencyList, char routeStart);

void do_stage1(Node_t **adjacencyList, char routeStart);

/* end function prototypes ---------------------------------------------------*/


char readStart(char *str) {
    assert(str);
    return str[0];
}

int vertex2Index(char vertex) {
    if (isupper(vertex)) {
        return (int) vertex - ASCII_CONVERTOR_UPPERCASE;
    } else {
        return (int) vertex - ASCII_CONVERTOR_LOWERCASE;
    }
}

void printMultigraph(Node_t **adjacencyList) {
    int i;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (adjacencyList[i] != NULL) {
            Node_t *current = adjacencyList[i];
            printf("%c:\t", current->data.startVertex);
            while (current != NULL) {
                printf("%c %c %d ID= %d S= %d --> ", current->data.startVertex,
                       current->data.endVertex, current->data.value,
                       current->data.ID, current->data.status);
                current = current->next;
            }
            printf("\n");
        }
    }
}

void resetStatus(Node_t **adjacencyList) {
    int i;
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
    if (list == NULL)
        return NULL;
    Node_t *new = (Node_t *) malloc(sizeof(Node_t));
    new->data = list->data;
    new->next = cloneList(list->next);
    return new;
}

void cloneAdjacencyList(Node_t **dest, Node_t **src) {
    int i;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (src[i] != NULL) {
            dest[i] = cloneList(src[i]);
        }
    }
}

void updateExtremeEdge(int edgeCount, int curr_value, int *max, int *min) {
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
    char start, end;
    int value = 0;
    /* Multigraph properties */
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
    return 0;
}

incident_t incidentPacker(char startVertex, char endVertex, int value, int ID) {
    /* pack up incident */
    incident_t curr_incident;
    curr_incident.startVertex = startVertex;
    curr_incident.endVertex = endVertex;
    curr_incident.value = value;
    curr_incident.ID = ID;
    curr_incident.status = UNVISITED;
    return curr_incident;
}

void checkType(int oddDegree, int evenDegree) {
    if (!oddDegree && evenDegree) {
        printf("S0: Multigraph is Eulerian");
        return;
    } else if (oddDegree == 2) {
        printf("S0: Multigraph is traversable\n");
    }
    exit(EXIT_FAILURE);
}

int countVertices(Node_t **adjacencyList) {
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
    int i;
    /* free every non-empty linked lists in AdjacencyList*/
    for (i = 0; i < MAX_VERTICES; i++) {
        deleteList(&AdjacencyList[i]);
    }
}

void append(Node_t **head, data_t new_data) {
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

void printOutput(Node_t **circuit, int *lineCount) {
    /* static prevLine keeps track of last line printed */
    static int prevLine = A_NEGATIVE_NUMBER;
    if ((*lineCount <= FIRST_TEN_LINE || !(*lineCount % 5)) && prevLine !=
                                                               *lineCount) {
        /* Call printRestrictedCircuit to print formatted output */
        printRestrictedCircuit(circuit);
        prevLine = *lineCount;
    }
    /* increment of lineCount by 1 */
    (*lineCount)++;
}

void printRestrictedCircuit(Node_t **circuit) {
    assert (circuit != NULL && *circuit != NULL);
    Node_t *curr = *circuit;
    /* print start vertex of the route */
    printf("S1: %c", curr->data.startVertex);
    /* count how many edge are visited in the circuit */
    int edgeCount = countList(circuit);
    int printCount = 0;
    while (curr != NULL) {
        if (edgeCount > PRINT_EDGE_LIMIT) {
            if (printCount < 6) {
                printf("-%d->%c", curr->data.value, curr->data.endVertex);
            } else if (printCount == 6) {
                printf("...");
            } else if (printCount == edgeCount - 7) {
                printf("%c", curr->data.endVertex);
            } else if (printCount >= (edgeCount - 6)) {
                printf("-%d->%c", curr->data.value, curr->data.endVertex);
            }
            printCount++;
        } else if (edgeCount <= PRINT_EDGE_LIMIT) {
            printf("-%d->%c", curr->data.value, curr->data.endVertex);
        }
        curr = curr->next;
    }
    printf("\n");
}

void printList(Node_t **head) {
    assert(head != NULL && *head != NULL);
    Node_t *current = *head;
    while (current != NULL) {
        printf("%c %c %d ID= %d S= %d --> ", current->data.startVertex,
               current->data.endVertex, current->data.value,
               current->data.ID, current->data.status);
        current = current->next;
    }
    printf("\n");
}

char selectVertex(Node_t **adjacencyList, Node_t **circuit) {
    assert(adjacencyList != NULL && *adjacencyList != NULL);
    assert(circuit != NULL && *circuit != NULL);
    Node_t *currIncident = *circuit;
    char smallest = A_NEGATIVE_NUMBER;
    while (currIncident != NULL) {
        char start = currIncident->data.startVertex;
        int startIndex = vertex2Index(start);
        Node_t *head = adjacencyList[startIndex];
        if (hasUnvisitedEdge(&head)) {
            smallest = start;
            break;
        }
        currIncident = currIncident->next;
    }
    return smallest;
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
    Node_t *curr = node;
    int *edgeID = key;
    if (curr->data.ID == *edgeID) {
        return 1;
    }
    return 0;
}

int searchStartVertex(void *node, void *key) {
    Node_t *curr = node;
    char *startVertex = key;
    if (curr->data.startVertex == *startVertex) {
        return 1;
    }
    return 0;
}

void deleteList(struct Node **head) {
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
    printf("\nStage %d Output\n--------------\n", stage_no);
}

Node_t *findMinValue(Node_t **head) {
    /* Assert real head is not NULL */
    assert(head != NULL && (*head) != NULL);
    Node_t *current = *head;
    Node_t *min = NULL;

    while (current != NULL) {
        /* Assign a incident contains a unvisited edge to current */
        if (current->data.status && min == NULL) {
            min = current;
        } else if (current->data.status) {
            if (current->data.value < min->data.value) {
                min = current;
            }
                /* select an edge that leads to a vertex represented by a
                 * character with the smallest ASCII code. */
            else if (current->data.value == min->data.value &&
                     current->data.endVertex < min->data.endVertex) {
                min = current;
            }
        }
        current = current->next;
    }
    assert(min != NULL);
    return min;
}

int updateStatus(Node_t **adjacencyList, Node_t *min) {
    /* if given edge has been visited, do nothing */
    char nextVertex = min->data.startVertex;
    Node_t* curr = adjacencyList[vertex2Index(nextVertex)];
    while (curr!=NULL){
        if (curr->data.ID == min->data.ID){
            curr->data.status = VISITED;
        }
        curr = curr->next;
    }

    nextVertex = min->data.endVertex;
    curr = adjacencyList[vertex2Index(nextVertex)];
    while (curr!=NULL){
        if (curr->data.ID == min->data.ID){
            curr->data.status = VISITED;
        }
        curr = curr->next;
    }
    return 0;
}

void printIncident(incident_t incident) {
    printf("start= %c | end= %c | value= %d | ID= %d | status =%d \n",
           incident.startVertex, incident.endVertex, incident.value, incident
                   .ID, incident.status);
}

char appendIncident(Node_t **adjacencyList, Node_t **walk, char nextVertex) {
    assert(walk != NULL);
    int nextIndex = vertex2Index(nextVertex);
    /*  find edge with minimum value for current vertex */
    Node_t *min = findMinValue(&adjacencyList[nextIndex]);
    /* change status of a pair of incidents to visited */
    updateStatus(adjacencyList, min);
    nextVertex = min->data.endVertex;
    /* compute index of the next vertex */
    append(walk, min->data);
    return nextVertex;
}

Node_t *constructCircuit(Node_t **adjacencyList, char routeStart) {
    char nextVertex = routeStart;
    Node_t *walk = NULL;
    int circuitCompleted = 0;
    /* Compute index of start point */
    int nextIndex = vertex2Index(nextVertex);
    /* keep moving till
         * reached the start vertex, the circuit is completed
         * no unvisited edge found for current vertex */
    while (!circuitCompleted && hasUnvisitedEdge
            (&adjacencyList[nextIndex])) {
        /* Update nextVertex */
        nextVertex = appendIncident(adjacencyList, &walk, nextVertex);
        if (nextVertex == routeStart) {
            circuitCompleted++;
        }
    }
    return walk;
}

void do_stage0(Node_t **adjacencyList, char routeStart) {
    printOutputHeader(0);
    readMultigraph(adjacencyList, routeStart);
}

void do_stage1(Node_t **adjacencyList, char routeStart) {
    printOutputHeader(1);
    char nextVertex;
    int lineCount = 0;
    /* Linked list that records incident travelled */
    Node_t *circuit = NULL;
    Node_t *joint = NULL;
    /* Construct initial circuit */
    ConstructInitialCircuit(adjacencyList, &circuit, routeStart);
    printOutput(&circuit, &lineCount);

    /* keeping constructing new circuit until all edge were visited */
    while ((nextVertex = selectVertex(adjacencyList, &circuit))!= A_NEGATIVE_NUMBER) {
        Node_t* walk = NULL;
        walk = constructCircuit(adjacencyList, nextVertex);
        joint = searchList(&circuit, &nextVertex, &searchStartVertex);
        insertBefore(&circuit, &walk, &joint);
        printOutput(&circuit, &lineCount);
    }
    printf("S1: Scenic route value is %d", computeScenicValue(&circuit));
    /* free circuit */
    deleteList(&circuit);
    circuit = NULL;
}

void resetSandbox(Node_t **adjacencyList, Node_t **circuit,
                  Node_t **adjacencyListCopy, Node_t **circuitCopy) {
    cloneAdjacencyList(adjacencyListCopy, adjacencyList);
    *circuitCopy = cloneList(*circuit);
}

void ConstructInitialCircuit(Node_t** adjacencyList, Node_t** circuit, char
                             routeStart){
    Node_t *joint = NULL;
    /* Construct initial circuit */
    Node_t *walk = constructCircuit(adjacencyList, routeStart);
    insertBefore(circuit, &walk, &joint);
}

void updateAdjacencyList(Node_t **adjacencyList, Node_t** circuit){
    Node_t* incident = *circuit;
    Node_t* real;
    while (incident!=NULL){
        updateStatus(adjacencyList, incident);
        incident = incident->next;
    }
}
void findBestExtension(Node_t **adjacencyList, Node_t** circuit){
    Node_t *joint = NULL;
    Node_t* best = NULL;
    int maxVal = 0;

    /* To test each possible extension, make a copy of adjacencyList */
    Node_t *copy[MAX_VERTICES] = {NULL};
    Node_t *localCircuit = NULL;
    resetSandbox(adjacencyList, circuit, copy, &localCircuit);

    char nextVertex;
    int nextIndex;
    char bestnextVertex;
    Node_t* walk = NULL;

    Node_t *currIncident = *circuit;
    /* traverse each vertex */
    while (currIncident != NULL) {
        nextVertex = currIncident->data.startVertex;
        nextIndex = vertex2Index(nextVertex);
        int currVal = 0;
        if (hasUnvisitedEdge(&copy[nextIndex])) {
            printf("nextVertex: %c",nextVertex);
            walk = constructCircuit(copy, nextVertex);
            printRestrictedCircuit(&walk);
            joint = searchList(&localCircuit, &nextVertex, &searchStartVertex);
            insertBefore(&localCircuit, &walk, &joint);
            printRestrictedCircuit(&localCircuit);
            if ((currVal=computeScenicValue(&localCircuit))>maxVal){
                maxVal = currVal;
                best = walk;
                printf("currBest= ");
                printRestrictedCircuit(&best);
                bestnextVertex = nextVertex;
            }
            deleteList(&walk);
            walk = NULL;
            /* reset localCircuit and copy for next iteration */
            resetSandbox(adjacencyList, circuit, copy, &localCircuit);
        }
        currIncident = currIncident->next;
    }

    Node_t *lastNode = localCircuit;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }
    nextVertex = lastNode->data.endVertex;
    nextIndex = vertex2Index(nextVertex);
    int currVal = 0;
    if (hasUnvisitedEdge(&copy[nextIndex])) {
        walk = constructCircuit(copy, nextVertex);
        printf("nextVertex: %c",nextVertex);
        printRestrictedCircuit(&walk);
        mergeList(&localCircuit, &walk);
    }

    if ((currVal=computeScenicValue(&localCircuit))>maxVal){
        maxVal = currVal;
        best = walk;
        mergeList(circuit, &walk);
    }
    else {
        joint = searchList(circuit, &bestnextVertex, &searchStartVertex);
        insertBefore(circuit, &best, &joint);
    }
    printf("Best= ");
    printRestrictedCircuit(&best);
    updateAdjacencyList(adjacencyList, circuit);
}

void do_stage2(Node_t **adjacencyList, char routeStart) {
    printOutputHeader(2);
    /* Linked list that records incident travelled */
    Node_t *circuit = NULL;

    ConstructInitialCircuit(adjacencyList, &circuit, routeStart);
    findBestExtension(adjacencyList, &circuit);
    findBestExtension(adjacencyList, &circuit);

    /* free circuit */
    deleteList(&circuit);
    circuit = NULL;
}

void mergeList(Node_t **lst1, Node_t **lst2) {
    assert(lst1 != NULL && lst2 != NULL);
    assert(*lst1 != NULL && *lst1 != NULL);
    Node_t *end = *lst1;
    /* locate the last node of list 1 */
    while (end->next != NULL) {
        end = end->next;
    }
    end->next = *lst2;
}

void insertBefore(Node_t **circuit, Node_t **walk, Node_t **joint) {
    assert(circuit != NULL);
    assert(*walk != NULL && walk != NULL);
    /* if given linked list is empty */
    if (*circuit == NULL) {
        *circuit = *walk;
        return;
    }
    /* if given linked list to be inserted is empty */
    if (*walk == NULL) {
        return;
    }
    Node_t *curr = *circuit;
    Node_t *prev = NULL;
    while (curr->data.ID != (*joint)->data.ID) {
        prev = curr;
        curr = curr->next;
    }
    Node_t *walkHead = *walk;
    Node_t *walkEnd = NULL;
    curr = *walk;

    while (curr != NULL) {
        walkEnd = curr;
        curr = (curr)->next;
    }
    /* if list is to be inserted at the front of circuit */
    if (prev == NULL) {
        *circuit = walkHead;
        walkEnd->next = (*joint);
    }
        /* if list is to be inserted at the middle of the circuit */
    else {
        prev->next = *walk;
        walkEnd->next = (*joint);
    }
}

int computeScenicValue(Node_t **circuit) {
    Node_t *curr = *circuit;
    int coeff = 1;
    int value = 0;
    while (curr != NULL) {
        value += (curr->data.value) * coeff;
        coeff++;
        curr = curr->next;
    }
    return value;
}

int main(int argc, char *argv[]) {
    Node_t *adjacencyList[MAX_VERTICES] = {NULL};
    char routeStart = readStart(argv[1]);
    do_stage0(adjacencyList, routeStart);
//    do_stage1(adjacencyList, routeStart);
    resetStatus(adjacencyList);
    do_stage2(adjacencyList, routeStart);
    freeAdjacencyList(adjacencyList);
    return 0;
}
/* Algorithms are fun */

