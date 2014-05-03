// Julian Sparber 2014
// Projcet for ASD summer 2014

#include <stdio.h>
#include <stdlib.h>
#define true 1
#define false 0
#define STRING_LENGTH 32
typedef struct tArc {
	char node[STRING_LENGTH];
	double distance[3];
	struct tArc *next;
} tArc;

typedef struct tNode {
	char node[STRING_LENGTH];
	tArc *arc;
	struct tNode *next;
} tNode;


int getDistance(FILE *, double[]);
int addArc(tArc **, tArc *);
int addNode(tNode **, tNode *);
int getNodeName(FILE *, char *);
int loadDB(FILE *);

int main () {
	FILE *inputDB;
	inputDB = fopen("database.txt","r"); 
	loadDB(inputDB);

	fclose(inputDB);
	return 0;
}

int loadDB(FILE *inputDB) {
	int i, j;
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
			getNodeName(inputDB, newNode->node);
			getNodeName(inputDB, newArc->node);
			getDistance(inputDB, newArc->distance);
			addArc(&newNode->arc, newArc);
		/*printf("Arc:\n");
		for (j = 0; j < STRING_LENGTH && newArc->node[j] != '\n'; j++)
			printf("%c", newArc->node[j]);
			printf("\n");
		*/
		}
		addNode(&DataHead, newNode);
	}
	/*for (i = 0; i < STRING_LENGTH && DataHead->next->node[i] != '\n'; i++)
		printf("%c", DataHead->next->node[i]);
*/
	printf("%lf %lf %lf", DataHead->next->arc->distance[0], DataHead->arc->distance[1], DataHead->arc->distance[2]);
	printf("\n");
	return error;
}

int printNode(
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
	fscanf(inputDB, "%lf %lf %lf\n", &distance[0], &distance[1], &distance[2]);
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
