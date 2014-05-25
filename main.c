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
	tNode *DataHead = NULL;
	tNode *newNode = NULL;
	tArc *newArc = NULL;
	fscanf(inputDB, "%d", &maxNode);

	if (fgetc(inputDB) != '\n')
		error = true;

	/*printf("Max Nodes: %d \n", maxNode);*/
	for (node = 0; node < maxNode; node++) {
		fscanf(inputDB, "%d\n", &maxArc);
		/*printf("Max Arcs: %d\n", maxArc);*/
		newNode = (tNode *)malloc(sizeof(tNode));
    newNode->minDistance = 0.0;
		newNode->next = NULL;
		for (i = 0; i < maxArc; i++) {
			newArc = (tArc *)malloc(sizeof(tArc));
			newArc->dest = NULL;
			getNodeName(inputDB, newNode->node);
			getNodeName(inputDB, newArc->node);
			getDistance(inputDB, newArc->distance);
			addArc(&newNode->arc, newArc);
		}
		addNode(&DataHead, newNode);
	}
	printf("Got this Data:\n");
	printAllNode(DataHead);
	return (!error) ? DataHead : NULL;
}

double calcElMedian(tList *nodeList, int median) {
  printf("Start function median: %d\n", median);
  /*should be random*/
  int x = listLength(nodeList);
	double randomMinDistance = getMinDistance(nodeList, (int)x/2);

  printf("Random Element: [%d], distance %lf\n", x/2, randomMinDistance);
  tList * minorList = NULL;
  tList * equalList = NULL;
  tList * majorList = NULL;
  tList * listEl;
	int lengthMinorList;
	int lengthEqualList;
	int lengthMajorList;

	/*printf("2Test%lf\n", randomMinDistance );*/
  for (listEl = nodeList; listEl != NULL; listEl = listEl->next) {
	/*printf("Test i: %d\n", i);*/
    /*printf("%s\n", listEl->node->node);*/
    if (listEl->node->minDistance < randomMinDistance)
      addList(&minorList, listEl->node);
    else if (listEl->node->minDistance == randomMinDistance)
      addList(&equalList, listEl->node);
    else if (listEl->node->minDistance > randomMinDistance)
      addList(&majorList, listEl->node);
  }
	lengthMinorList = listLength(minorList);
	lengthEqualList = listLength(equalList);
	lengthMajorList = listLength(majorList);
  printf("MinorList\n");
  for (listEl = minorList; listEl != NULL; listEl = listEl->next)
    printf("El: %lf\n", listEl->node->minDistance);
  printf("EqualList\n");
  for (listEl = equalList; listEl != NULL; listEl = listEl->next)
    printf("El: %lf\n", listEl->node->minDistance);
  printf("MajorList\n");
  for (listEl = majorList; listEl != NULL; listEl = listEl->next)
    printf("El: %lf\n", listEl->node->minDistance);
	printf("Lengths: %d, %d, %d\n", lengthMinorList, lengthEqualList, lengthMajorList);
  printf("Median: %d\n", median);
  if (median <= lengthMinorList) {
      printf("Minor\t\thello\n");
      return calcElMedian(minorList, median);
  }
  else if (median > lengthMinorList + lengthEqualList) {
      printf("Major: %d \t\thello\n",median - lengthMinorList - lengthEqualList);
      return calcElMedian(majorList, median - lengthMinorList - lengthEqualList);
  }
	else {
    printf("Other: %lf\t\thello\n", randomMinDistance);
		return randomMinDistance;
  }
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
    /*printf("newList");*/
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
  double averageDistance;
  printf("All nodes\n");
  for (el = dataHead, i = 0; el != NULL; el = el->next, i++) {
    averageDistance += el->minDistance;
    printf("Distance %s: %lf\n", el->node, el->minDistance);
  }
  averageDistance /= i;
     
  printf("Average Distance is: %lf\n", averageDistance);
	//nodeList = createList(dataHead);
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
  /*for (node = headData; node != NULL; node = node->next)
    printf("%lf - ", node->minDistance);
  printf("\n");*/
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
		/*printNode(foundStartNode);
		printNode(foundEndNode);*/
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
	//tList *list;
  clearDistances(dataHead, startNode);
  notExploredList = createList(dataHead);
	/*for (list = notExploredList; list != NULL; list = list->next)
		printf("Node of not explored list %s", list->node->node);*/
  while (notExploredList != NULL) {
      node = removeMinListEl(&notExploredList);
			/*printf("Node token form the not explored list %s", node->node);*/
	//for (list = notExploredList; list != NULL; list = list->next)
		//printf("Node of not explored list %s", list->node->node);
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
        //printf("Found new min Distance");
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
  /*printf("minDistace %lf\n", minDistance);*/
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
   /*printf("After %lf\n", arc->dest->minDistance);*/
 }
 return 0;
} 

int findNodeByAddr(tList *head, tNode *node) {
	//printf("Searching ...\n");
	int res = false;

	if (head != NULL) {
  	if (head->node == node) {
			/*printf("Found ...\n");*/
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
		/*printf("Arc not NULL search for %s\n", arc->node);*/
		arc->dest = findNode(dataHead, arc->node);
		if (arc->next != NULL) {
			/*printf("Go to next Arc\n");*/
			insertArcDest(dataHead, node, arc->next);
		}
		else
			if (node->next != NULL && node->next->arc != NULL) {
				/*printf("Go to next Node\n");*/
				insertArcDest(dataHead, node->next, node->next->arc);
			}
	}
	return 0;
}

tNode * findNode(tNode *node, char nodeName[]) {
	tNode * foundNode = NULL;
	if (strcmp(nodeName, node->node) == 0) {
		/*printf("Found Node: %s", nodeName);
		printNode(node);*/
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
	if ( *DataHead == NULL ) {
		*DataHead = newNode;
		/*printf("First Element added\n");*/
	}
	else {
		for (curEl = *DataHead; curEl->next != NULL; curEl = curEl->next);
		curEl->next = newNode;
		/*printf("New Element attached\n");*/
	}
	return 0;
}

int addArc(tArc **DataHead, tArc *newNode) {
	tArc *curEl;
	if ( *DataHead == NULL ) {
		*DataHead = newNode;
		/*printf("First Arc added\n");*/
	}
	else {
		for (curEl = *DataHead; curEl->next != NULL; curEl = curEl->next);
		curEl->next = newNode;
		/*printf("New Arc attached\n");*/
	}
	return 0;
}

int getDistance(FILE *inputDB, double distance[]) {
	fscanf(inputDB, "%lf %lf %lf\n",
		&distance[0], &distance[1], &distance[2]);
	return 0;
}

int getNodeName(FILE *inputDB, char node[]) {
	int i = 0;
	int error = false;
	char tmpChar;
	do {
		if (i > STRING_LENGTH - 2) {
			printf("To long Node Name\n");
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
