/* Julian Sparber 2014
 Projcet for ASD summer 2014 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
#define STRING_LENGTH 32
#define MAX_DISTANCE 255

typedef struct tArc {
	char node[STRING_LENGTH];
	double distance[3];
	struct tArc *next;
	struct tNode *dest;
} tArc;

typedef struct tNode {
	char node[STRING_LENGTH];
	tArc *arc;
	struct tNode *next;
  double minDistance;
	struct tNode *parent;
} tNode;

typedef struct tList {
  tNode *node;
  struct tList *next;
} tList;

int freeList(tList *);
double getMinDistance(tList *, int);
int addList(tList **, tNode *);
int listLength(tList *);
double calcElMedian(tList *, int);
int calcDistanceAverage(tNode *);
int readInput(char []);
int printResult(tNode *, tNode *);
tNode * removeMinListEl(tList **);
int calcDistance(tArc *, tNode *, int); 
tList * createList(tNode *);
int findNodeByAddr(tList *, tNode *);
int removeNode(tNode **, tNode *); 
int removeHead(tNode **);
int insertArcDest(tNode *, tNode *, tArc *);
int exploreNodes(tNode *, tNode*, int);
int calcRoute(tNode *, char [], char [], int);
tNode * findNode(tNode*, char []);
int printArc(tArc *);
int printNode(tNode *);
int printAllNode(tNode *);
int getDistance(FILE *, double[]);
int addArc(tArc **, tArc *);
int addNode(tNode **, tNode *);
int getNodeName(FILE *, char *);
tNode* loadDB(FILE *);

int main () {
	FILE *inputDB;
  char startNode[STRING_LENGTH];
  char endNode[STRING_LENGTH];
  int typology;
	tNode *dataHead = NULL;
	inputDB = fopen("database.txt","r"); 
	dataHead = loadDB(inputDB);
	fclose(inputDB);
  if (dataHead != NULL) {
    insertArcDest(dataHead, dataHead, dataHead->arc);
    printf("Enter the start node\n");
    readInput(startNode);
    printf("Enter the end node\n");
    readInput(endNode);
    printf("Enter the typology\n");
    scanf("%d", &typology);
    /*printf("'%s''%s'\n", startNode, endNode);*/
    calcRoute(dataHead, startNode, endNode, typology);
    calcDistanceAverage(dataHead);
    for (dataHead = dataHead; dataHead != NULL; dataHead = dataHead->next)
      free(dataHead);
  }
  return 0;
}


int readInput(char string[]) {
  int i;
  for (i = 0; i < STRING_LENGTH && string[i-1] != '\n'; i++)
    string[i] = getchar();
  string[i-1] = '\0';
  return 0;
}
tNode* loadDB(FILE *inputDB) {
  int i;
  int error = false;
  int maxNode = 0; 
  int node;
  int maxArc;
  int res;
  tNode *DataHead = NULL;
  tNode *newNode = NULL;
  tArc *newArc = NULL;
  res = fscanf(inputDB, "%d", &maxNode);
  if (res == 0) {
    printf("Error: wrong max node nummber\n");
    error = true;
  }

  if (fgetc(inputDB) != '\n')
    error = true;

  for (node = 0; node < maxNode && !error; node++) {
    res = fscanf(inputDB, "%d\n", &maxArc);
    if (res == 0) {
      printf("Error: wrong max arc nummber\n");
      error = true;
    }
    else {
      newNode = (tNode *)malloc(sizeof(tNode));
      newNode->minDistance = 0.0;
      newNode->next = NULL;
      for (i = 0; i < maxArc && !error; i++) {
        newArc = (tArc *)malloc(sizeof(tArc));
        newArc->dest = NULL;
        getNodeName(inputDB, newNode->node);
        if (newNode->node[0] != 'v') {
          printf("Error: wrong node name: %s\n", newNode->node);
          error = true;
          free(newNode);
        }
        else {
          getNodeName(inputDB, newArc->node);
          if (newArc->node[0] != 'v') {
            printf("Error: wrong arc destination of %s\n", newNode->node);
            error = true;
            free(newNode);
          }
          else {
            error = getDistance(inputDB, newArc->distance);
            if (error != true)
              addArc(&newNode->arc, newArc);
            else
              free(newNode);
          }
        }
      }
      addNode(&DataHead, newNode);
    }
	}
  if (!error) {
	  printf("Got this Data:\n");
	  printAllNode(DataHead);
  }
	return (!error) ? DataHead : NULL;
}

double calcElMedian(tList *nodeList, int median) {
  /*should be random*/
  int x = listLength(nodeList);
	double randomMinDistance = getMinDistance(nodeList, (int)x/2);

  tList * minorList = NULL;
  tList * equalList = NULL;
  tList * majorList = NULL;
  tList * listEl;
	int lengthMinorList;
	int lengthEqualList;

  for (listEl = nodeList; listEl != NULL; listEl = listEl->next) {
    if (listEl->node->minDistance < randomMinDistance)
      addList(&minorList, listEl->node);
    else if (listEl->node->minDistance == randomMinDistance)
      addList(&equalList, listEl->node);
    else if (listEl->node->minDistance > randomMinDistance)
      addList(&majorList, listEl->node);
  }
	lengthMinorList = listLength(minorList);
	lengthEqualList = listLength(equalList);
  if (median <= lengthMinorList) {
      freeList(equalList); 
      freeList(majorList);
      return calcElMedian(minorList, median);
  }
  else if (median > lengthMinorList + lengthEqualList) {
      freeList(minorList);
      freeList(equalList); 
      return calcElMedian(majorList, median - lengthMinorList - lengthEqualList);
  }
	else {
    freeList(minorList);
    freeList(equalList); 
    freeList(majorList);
		return randomMinDistance;
  }
  return 0;
}

int freeList(tList * list) {
  if (list != NULL)
    freeList(list->next);
    free(list);
  return 0;
}
double getMinDistance(tList *nodeList, int x) {
	int i = 1;
	tList *listEl = nodeList;
	while (listEl != NULL && i < x) {
		i++;
		listEl = listEl->next;
	}
	return listEl->node->minDistance;
}
int listLength(tList *list) {
	if(list == NULL)
		return 0;
	else
		return listLength(list->next) + 1;
}

int addList(tList **list, tNode *node) {
	tList *listEl;
	tList *newListEl;
	newListEl = (tList *)malloc(sizeof(tList));
	newListEl->node = node;
	newListEl->next = NULL;
	if ( *list == NULL ) {
		*list = newListEl;
	}
	else {
		for (listEl = *list; listEl->next != NULL; listEl = listEl->next);
		listEl->next = newListEl;
	}
	return 0;
}
int calcDistanceAverage(tNode * dataHead) {
  tNode * el;
	tList *nodeList = NULL;
  int length;
  double median;
  int i;
  double averageDistance = 0;
  for (el = dataHead, i = 0; el != NULL; el = el->next, i++) {
    averageDistance += el->minDistance;
  }
  averageDistance /= i;
     
  printf("Average Distance is: %lf\n", averageDistance);
  for (el = dataHead; el != NULL; el = el->next) {
    if (el->minDistance != 0.0)
      addList(&nodeList, el);
  }

  length = listLength(nodeList);
  if (length % 2 == 0)
    median = (calcElMedian(nodeList, length/2) + calcElMedian(nodeList, length/2 + 1)) / 2;
  else
	  median = calcElMedian(nodeList, length/2 + 1);
  printf("Median is %lf\n", median);
  return 0;
}

int clearDistances(tNode *headData, tNode *endNode) {
  tNode *node;
  for (node = headData; node != NULL; node = node->next) {
    node->minDistance = MAX_DISTANCE;
		node->parent = NULL;
	}
  endNode->minDistance = 0.0;
  return 0;
}

int calcRoute(
	tNode *headData, char startNode[], char endNode[], int typology) {
	tNode *foundStartNode;
	tNode *foundEndNode;
	foundStartNode = findNode(headData, startNode);
	foundEndNode = findNode(headData, endNode);
	if (foundStartNode != NULL && foundEndNode != NULL) {
		printf("Start and end Node found!\n");
		printf("Start routing\n");
    exploreNodes(headData, foundStartNode, typology);
		printResult(foundStartNode, foundEndNode);
	}
	else
		printf("Start or end Node not found!\n");

	return 0;
}

int printResult(tNode *startNode, tNode *endNode) {
	tNode *node;
	printf("Route:\n");
	for (node = endNode; node != startNode; node = node->parent)
		printf("\t%s\n", node->parent->node);
	printf("Result Distance is: %lf\n", endNode->minDistance);
	return 0;
}

int exploreNodes(tNode * dataHead, tNode * startNode, int typology) {
  tNode *node;
  tArc *arc;
  tList *notExploredList;
  clearDistances(dataHead, startNode);
  notExploredList = createList(dataHead);
  while (notExploredList != NULL) {
      node = removeMinListEl(&notExploredList);
    for (arc = node->arc; arc != NULL; arc = arc->next) {
     if (findNodeByAddr(notExploredList, arc->dest) == true) { 
       calcDistance(arc, node, typology);
     }
    }
  }

  return 0;
}
tNode * removeMinListEl(tList **list) {
  tNode *node = NULL; 
  tList *el;
  tList *precEl;
  double minDistance = MAX_DISTANCE;
  for (el = *list; el != NULL; el= el->next) {
      if (minDistance >= el->node->minDistance) {
        minDistance = el->node->minDistance;
        node = el->node;
      }
  }
	el = *list;
	precEl = el;
	while (el->node != node && el != NULL) {
		precEl = el;
		el = el->next;
	}
	if (el == *list && el->next == NULL)
		*list = NULL;
	if (el == *list)
		*list = el->next;
	else
		precEl->next = el->next;
	free(el);
  return node;
}

tList * createList(tNode *dataHead) {
  tNode *node;
  tList *firstListEl = NULL;
  tList *lastListEl = NULL;
  tList *newElList;
  for (node = dataHead; node != NULL; node = node->next) {
		newElList = (tList *)malloc(sizeof(tList));
    newElList->node = node;
    newElList->next = NULL;
    if (firstListEl == NULL) {
      firstListEl = newElList;
			lastListEl = firstListEl;
		}
    else {
      lastListEl->next = newElList;
			lastListEl = lastListEl->next;
		}
  }
  return firstListEl;
}
int calcDistance(tArc *arc, tNode *node, int typology) {
 if (arc->dest->minDistance > node->minDistance + arc->distance[typology]) {
   arc->dest->minDistance = node->minDistance + arc->distance[typology];
	 arc->dest->parent = node;
 }
 return 0;
} 

int findNodeByAddr(tList *head, tNode *node) {
	int res = false;

	if (head != NULL) {
  	if (head->node == node) {
    	res = true;
		}
  	else {
    	if (head->next == NULL)
      	res = false;
    	else {
      	res = findNodeByAddr(head->next, node);
			}
		}
	}
  return res;
}

int insertArcDest(tNode * dataHead, tNode * node, tArc * arc) {
	if (arc != NULL) {
		arc->dest = findNode(dataHead, arc->node);
		if (arc->next != NULL) {
			insertArcDest(dataHead, node, arc->next);
		}
		else
			if (node->next != NULL && node->next->arc != NULL) {
				insertArcDest(dataHead, node->next, node->next->arc);
			}
	}
	return 0;
}

tNode * findNode(tNode *node, char nodeName[]) {
	tNode * foundNode = NULL;
	if (strcmp(nodeName, node->node) == 0) {
		foundNode = node;
	}
	else
		if (node->next != NULL)
			foundNode = findNode(node->next, nodeName);
		else
			foundNode = NULL;
	return foundNode;
}

int printAllNode(tNode *node) {
		printNode(node);
	if (node->next != NULL)
		printAllNode(node->next);
	return 0;
}
int printNode(tNode *node) {
	int i;
	printf("Arcs of: ");
	for (i = 0; i < STRING_LENGTH && node->node[i] != '\n'; i++)
		printf("%c", node->node[i]);
	printf("\n");
	printArc(node->arc);
	return 0;
}

int printArc(tArc *arc) {
	int i;
	printf("\t");
	for (i = 0; i < STRING_LENGTH && arc->node[i] != '\n'; i++)
		printf("%c", arc->node[i]);
	printf(" with distances: ");
	printf("%.2lf %.2lf %.2lf",
		arc->distance[0], arc->distance[1], arc->distance[2]);
	printf("\n");
	if (arc->next != NULL)
		printArc(arc->next);
	return 0;
}

int addNode(tNode **DataHead, tNode *newNode) {
	tNode *curEl;
	if (*DataHead == NULL) {
		*DataHead = newNode;
	}
	else {
		for (curEl = *DataHead; curEl->next != NULL; curEl = curEl->next);
		curEl->next = newNode;
	}
	return 0;
}

int addArc(tArc **DataHead, tArc *newNode) {
	tArc *curEl;
	if ( *DataHead == NULL ) {
		*DataHead = newNode;
	}
	else {
		for (curEl = *DataHead; curEl->next != NULL; curEl = curEl->next);
		curEl->next = newNode;
	}
	return 0;
}

int getDistance(FILE *inputDB, double distance[]) {
  int res;
	res = fscanf(inputDB, "%lf %lf %lf\n",
		&distance[0], &distance[1], &distance[2]);

	return (res == 3) ? 0 : true;
}

int getNodeName(FILE *inputDB, char node[]) {
	int i = 0;
	int error = false;
	char tmpChar;
	do {
		if (i > STRING_LENGTH - 2) {
			printf("To long node name\n");
			error = true;
			while ( tmpChar != ' ') {
				tmpChar = fgetc(inputDB);
			}
		}
		else {
			node[i] = fgetc(inputDB);
		}
		i++;
	}
	while (node[i - 1] != ' ' && node[i - 1] != '\n' && !error);
	node[i - 1] = '\0';

	return error;
}
