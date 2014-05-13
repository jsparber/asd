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
} tNode;


int calcDistance(tArc *, tNode *, int); 
int findNodeByAddr(tNode *, tNode *);
int removeNode(tNode *, tNode *);
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
	tNode *dataHead = NULL;
	inputDB = fopen("database.txt","r"); 
	dataHead = loadDB(inputDB);
	fclose(inputDB);
	insertArcDest(dataHead, dataHead, dataHead->arc);
	calcRoute(dataHead, "v_a\n", "v_d\n", 0);

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

	printf("Max Nodes: %d \n", maxNode);
	for (node = 0; node < maxNode; node++) {
		fscanf(inputDB, "%d\n", &maxArc);
		printf("Max Arcs: %d\n", maxArc);
		newNode = (tNode *)malloc(sizeof(tNode));
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

int clearDistances(tNode *headData, tNode *startNode) {
  startNode->minDistance = 0;
  tNode *node;
  for (node = headData->next; node != NULL; node = node->next)
    node->minDistance = MAX_DISTANCE;
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
		printNode(foundStartNode);
		printf("Start routing\n");
    exploreNodes(headData, foundStartNode, typology);

	}
	else
		printf("Start or end Node not found!\n");

	printf("Result Distance is: %lf\n", foundEndNode->minDistance);
	return 0;
}

int exploreNodes(tNode * head, tNode * startNode, int typology) {
  tNode * node = head;
  tArc * arc;
  tNode *explored = NULL;
  tNode *notExplored = head;
  clearDistances(head, startNode);
  while (notExplored != NULL) {
   removeNode(notExplored, node); 
   addNode(&explored, node);
   for (arc = node->arc; arc != NULL; arc = arc->next) {
     if (findNodeByAddr(explored, arc->dest))
       calcDistance(arc, node, typology);
   }
   node = node->next;
  }
  return 0;
}
int calcDistance(tArc *arc, tNode *node, int typology) {
 if (arc->dest->minDistance > node->minDistance + arc->distance[typology])
   arc->dest->minDistance = node->minDistance + arc->distance[typology];
 return 0;
} 
int findNodeByAddr(tNode *head, tNode *node) {
  if (head == node)
    return true;
  else
    if (head == NULL)
      return false;
    else
      findNodeByAddr(head->next, node);
  return false;
}
int removeNode(tNode *head, tNode * node) {
  if (head->next == node){
    head->next = head->next->next;
  }
  else
    removeNode(head->next, node);

  return 0;
}
int insertArcDest(tNode * dataHead, tNode * node, tArc * arc) {
	if (arc != NULL) {
		printf("Arc not NULL search for %s\n", arc->node);
		arc->dest = findNode(dataHead, arc->node);
		if (arc->next != NULL) {
			printf("Go to next Arc\n");
			insertArcDest(dataHead, node, arc->next);
		}
		else
			if (node->next != NULL && node->next->arc != NULL) {
				printf("Go to next Node\n");
				insertArcDest(dataHead, node->next, node->next->arc);
			}
	}
	return 0;
}

tNode * findNode(tNode *node, char nodeName[]) {
	tNode * foundNode = NULL;
	if (strcmp(nodeName, node->node) == 0) {
		printf("Found Node: %s", nodeName);
		printNode(node);
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
		printf("First Element added\n");
	}
	else {
		for (curEl = *DataHead; curEl->next != NULL; curEl = curEl->next);
		curEl->next = newNode;
		printf("New Element attached\n");
	}
	return 0;
}

int addArc(tArc **DataHead, tArc *newNode) {
	tArc *curEl;
	if ( *DataHead == NULL ) {
		*DataHead = newNode;
		printf("First Arc added\n");
	}
	else {
		for (curEl = *DataHead; curEl->next != NULL; curEl = curEl->next);
		curEl->next = newNode;
		printf("New Arc attached\n");
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
	node[i - 1] = '\n';

	return error;
}
