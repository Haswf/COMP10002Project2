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
struct Node{
    data_t data;
    Node_t *next;
};
/* end type definitions ------------------------------------------------------*/

/* function prototypes -------------------------------------------------------*/

// list operations
void push(Node_t** head, data_t new_data);
void insertAfter(Node_t* prev_node, data_t new_data);
void append(Node_t** head, data_t new_data);
void deleteList(Node_t** head);
int countList(Node_t** head);
void freeAdjacencyList(Node_t** AdjacencyList);
incident_t incidentPacker(char startVertex, char endVertex, int value, int ID);
int countVertices(Node_t** adjacencyList);
void countDegree(Node_t**,int* , int*);
void printList(Node_t** head);
void printType(int oddDegree, int evenDegree);
void printOutputHeader(int stage_no);
void deleteByID(Node_t **head, int edgeID);
void printIncident(incident_t incident);
void testFindMinValue(Node_t **adjacencyList);
/* end function prototypes ---------------------------------------------------*/


char readStart(char* str){
    assert(str);
    return str[0];
}

int vertex2Index(char vertex){
    if (isupper(vertex)){
        return (int)vertex-ASCII_CONVERTOR_UPPERCASE;
    }
    else{
        return (int)vertex-ASCII_CONVERTOR_LOWERCASE;
    }
}

void printMultigraph(Node_t** adjacencyList) {
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

void updateExtremeEdge(int edgeCount, int curr_value, int* max, int* min){
    /* Initialize minVal, maxVal with the first incident value */
    if (!edgeCount){
        *max = *min = curr_value;
    }
    /* otherwise, update maxVal and minVal if necessary */
    else{
        if (curr_value > *max){
            *max = curr_value;
        }
        if (curr_value < *min){
            *min = curr_value;
        }
    }
}

int readMultigraph(Node_t** adjacencyList, char routeStart){
	char start, end;
	int value = 0;
	/* multigraph properties */
    int edgesCount, minVal, maxVal, sum, oddDegree, evenDegree;
    edgesCount = minVal = maxVal= sum = oddDegree = evenDegree = 0;


	while (scanf("%c %c %d\n", &start, &end, &value)==3){
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
    printType(oddDegree, evenDegree);
	return 0;
}

incident_t incidentPacker(char startVertex, char endVertex, int value, int ID){
    /* pack up incident */
    incident_t curr_incident;
    curr_incident.startVertex = startVertex;
    curr_incident.endVertex = endVertex;
    curr_incident.value = value;
    curr_incident.ID = ID;
    curr_incident.status = UNVISITED;
    return curr_incident;
}

void printType(int oddDegree, int evenDegree){
    if (!oddDegree && evenDegree) {
        printf("S0: Multigraph is Eulerian\n");
        return;
    }
    else if (oddDegree == 2){
        printf("S0: Multigraph is traversable\n");
    }
    exit(EXIT_FAILURE);
}

int countVertices(Node_t** adjacencyList){
    int count = 0;
    int i;
    for (i=0;i<MAX_VERTICES;i++) {
        if (adjacencyList[i] != NULL){
            count++;
        }
    }
    return count;
}

void countDegree(Node_t** adjacencyList, int* oddDegreeCount, int*
    evenDegreeCount){
    int i, degreeCount;
    for (i=0;i<MAX_VERTICES;i++){
        if (adjacencyList[i] != NULL){
            degreeCount = countList(&adjacencyList[i]);
            if (degreeCount % 2){
                (*oddDegreeCount)++;
            }
            else {
                (*evenDegreeCount)++;
            }
        }
    }
}

void freeAdjacencyList(Node_t** AdjacencyList){
    int i;
    /* free every non-empty linked lists in AdjacencyList*/
    for (i=0;i<MAX_VERTICES;i++){
        deleteList(&AdjacencyList[i]);
    }
}

Node_t* mergeList(Node_t** head1, Node_t** head2){
    /* Insert list2 at front of list1 */
    assert(head1!= NULL && head2!= NULL);

    if (*head2==NULL){
        *head2 = *head1;
        *head1 = NULL;
        return *head2;
    }

    if (*head1==NULL){
        return *head2;
    }
    Node_t* curr = *head2;
    Node_t* last = NULL;
    /* Find last node of list 2 */
    while (curr != NULL){
        last = curr;
        curr = curr->next;
    }
    /* link two lists */
    last->next = *head1;
    /* delete real head of list 2 */
    *head1 = NULL;
    return *head2;
}


void append(Node_t** head, data_t new_data){
    Node_t* new = (Node_t*)malloc(sizeof(Node_t));
    /* assert malloc is successful */
    assert(new);
    /* pointer to the last node */
    Node_t* last = *head;
    /* put in data */
    new->data = new_data;
    /* make next of the new node as NULL since this will be the last node*/
    new->next = NULL;

    /* if the Linked List is empty, then make the new node as head */
    if (*head == NULL){
        *head = new;
        return;
    }
    /* find the last node */
    while (last->next != NULL)
        last = last->next;
    /* change the next of last node to new */
    last->next = new;
}

void printWalk(char routeStart, Node_t** Walk){
    assert (Walk != NULL && *Walk != NULL);
    Node_t* curr = *Walk;
    printf("%c",routeStart);
    while (curr != NULL){
        printf("-%d->%c", curr->data.value, curr->data.endVertex);
        curr = curr->next;
    }
    printf("\n");
}

void printList(Node_t** head){
    assert(head!= NULL && *head!= NULL);
    Node_t* current = *head;
    while (current != NULL) {
        printf("%c %c %d ID= %d S= %d --> ", current->data.startVertex,
               current->data.endVertex, current->data.value,
               current->data.ID, current->data.status);
        current = current->next;
    }
}

Node_t* selectVertex(Node_t** adjacencyList) {
    int i;
    Node_t* vertexList = NULL;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (adjacencyList[i] != NULL && hasUnvisitedEdge(adjacencyList[i])){
            vertexList = adjacencyList[i];
        }
    }
    return vertexList;
}
int hasUnvisitedEdge(Node_t** head) {
    /* Assert head and *head are valid pointer */
    assert(head != NULL && *head != NULL);
    Node_t *curr = *head;
    /* traverse list till unvisited edge found */
    while (curr != NULL){
        if (curr->data.status){
            return FOUND;
        }
    }
    return NOT_FOUND;
}
Node_t* searchForID(Node_t **head, int edgeID){
    /* Assert the given head is non-empty */
    assert(head != NULL && *head != NULL);
    Node_t* curr = *head;

    while (curr != NULL){
        if (curr->data.ID == edgeID){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void deleteList(struct Node** head) {
    /* return if NULL pointer is given */
    if ((*head) == NULL || head == NULL){
        return;
    }
    else{
        /* dereference head to get the real head */
        Node_t* current = *head;
        Node_t* next;

        /* delete each node */
        while (current != NULL){
            next = current->next;
            free(current);
            current = next;
        }
        /* Set head to NULL */
        *head = NULL;
    }
}

int countList(Node_t** head){
    assert(head != NULL && *head != NULL);
    int count = 0;
    /* Initialize current to be head */
    Node_t* current = *head;
    while (current != NULL){
        count++;
        current = current->next;
    }
    return count;
}

void printOutputHeader(int stage_no){
    /* The print_output_header(int stage_no) prints stage header. */
    printf("\nStage %d Output\n--------------\n", stage_no);
}

Node_t* findMinValue(Node_t** head){
    /* Assert real head is not NULL */
    assert(head!=NULL && (*head)!=NULL);
    Node_t* current = *head;
    Node_t* min;
    /* Assign a incident contains a unvisited edge to current */
    while (current != NULL){
        if (current->data.status){
//            printf("Initial value: ");
//            printIncident(current->data);
//            printf("\n");
            /* initialize min with first unvisited incident found*/
            min = current;
            break;
        }
        current = current->next;
    }

    while (current != NULL){
        if (current->data.status){
            if (current->data.value < min->data.value){
                min = current;
            }
            /* select an edge that leads to a vertex represented by a
             * character with the smallest ASCII code. */
            else if (current->data.value == min->data.value){
                if (current->data.endVertex < min->data.endVertex){
                    min = current;
                }
            }
        }
        current = current->next;
    }
    return min;
}

int updateStatus(Node_t** adjacencyList, Node_t* min){
    min->data.status = VISITED;
    char nextVertex = min->data.endVertex;
    /* Search for another incident with the same ID */
    Node_t* tmp = searchForID(&adjacencyList[vertex2Index(nextVertex)],
                              min->data.ID);
    tmp->data.status = VISITED;
    assert(min->data.status == VISITED && tmp->data.status == VISITED);
        return 0;
}

void printIncident(incident_t incident){
    printf("start= %c | end= %c | value= %d | ID= %d | status =%d \n",
           incident.startVertex, incident.endVertex, incident.value, incident
                   .ID, incident.status);
}
void do_stage0(Node_t** adjacencyList, char routeStart){
    printOutputHeader(0);
    readMultigraph(adjacencyList, routeStart);
    printMultigraph(adjacencyList);
}

void do_stage1(Node_t** adjacencyList, char routeStart) {
    printOutputHeader(1);
    char nextVertex = routeStart;
    int StartIndex = vertex2Index(routeStart);
    /* Linked list that records incident travelled */
    Node_t* path = NULL;

    while (findMinValue(&adjacencyList[StartIndex])){
        Node_t *walk = NULL;
        int backToStartCount = 0;
        while (backToStartCount != 1) {
            int nextIndex = vertex2Index(nextVertex);
            Node_t *min = findMinValue(&adjacencyList[nextIndex]);
            /* change status of a pair of incidents to visited */
            updateStatus(adjacencyList, min);
            nextVertex = min->data.endVertex;
            append(&walk, min->data);
            if (nextVertex == routeStart) {
                backToStartCount++;
            }
        }
        path = mergeList(&path, &walk);
        printWalk(routeStart, &path);
    }
}

int main(int argc, char *argv[]) {
    Node_t* adjacencyList[MAX_VERTICES] = {NULL};
    char routeStart = readStart(argv[1]);
    do_stage0(adjacencyList, routeStart);
    do_stage1(adjacencyList, routeStart);
//    testFindMinValue(adjacencyList);
    freeAdjacencyList(adjacencyList);
    return 0;
}

void testFindMinValue(Node_t **adjacencyList) {
    Node_t *head = NULL;
    char start, end;
    int value = 0;
    int edgesCount = 0;
    while (scanf("%c %c %d\n", &start, &end, &value) == 3) {
        /* Call vertex2index to convert end from char to corresponding int*/
        /* Append new incident to the list */
        append(&head, incidentPacker(start, end, value, edgesCount));
        edgesCount++;
    }
    printList(&head);
    printIncident(findMinValue(&head)->data);
}
/* Algorithms are fun */

