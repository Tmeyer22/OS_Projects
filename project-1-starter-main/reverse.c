#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Doublely Linked List node struct
//Used to store file the read backwards to reverse file
struct node{

char* line;

struct node* previous;
struct node* next;

};

int main(int argc, char *argv[])
{
	//If there aren't the right number of files, print message and error using exit()
	if(argc < 2  || argc > 3){
		fprintf(stderr, "usage: reverse <input> <output>\n");
		exit(1);
	}
	//Initalize file pointer here for scope
	FILE *outputFile;
	//If there are two files run two file error checks
	if (argc == 3)
	{
		//Check if files are different
		if(strcmp(argv[1], argv[2]) == 0){
			fprintf(stderr, "error: input and output file must differ\n");
			exit(1);
		}
		//Check if file can be opened
		if ((outputFile = fopen(argv[2], "w")) == NULL)
		{
			fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
			exit(1);
		}
	}
	//Initalize input file pointer and check that file opens
	FILE *file1;
	if ((file1 = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
		exit(1);
	}

	//Inital variables for getline()
	char* buffer= NULL;
	size_t buffSize = 0;
	ssize_t lineOut;

	//Setup linked list head
	struct node* head = malloc(sizeof( struct node));
	head->next = NULL; //NULLs to prevent seg faults
	head->previous = NULL;
	head->line = NULL;
	//Initalize working and tail nodes
	struct node* curLine = head;
	struct node* holder = malloc(sizeof( struct node));
	struct node* tail = malloc(sizeof( struct node));
	tail->line = NULL;
	//Read file line by line
	while ((lineOut = getline(&buffer, &buffSize, file1)) != -1){
		//Holder point to set previous later
		holder->next = curLine;
		//Allocate new memory
		curLine->next = malloc(sizeof*(curLine->next));
		//Add next node
		curLine = curLine->next;
		curLine->previous = holder->next;
		curLine->line = strdup(buffer);
		curLine->next = NULL; 
	}
	//Set tail to close list
	tail->next = curLine;
	//If no output file given, print to termial
	if (argc == 2)
	{
		//Read through the list backwards
		struct node* cur = tail->next;
		while(cur->previous != NULL){
			printf("%s", cur->line );
			cur = cur->previous;
		}
	}
	//If output file givem, print to file instead
	else{
	struct node* cur = tail->next;
	while(cur->previous != NULL){
		fputs(cur->line, outputFile);
		cur = cur->previous;
	}
	fclose(outputFile);
	}
	//Free all nodes 
	while(head != NULL){
		struct node* temp = head;
		head = head->next;
		free(temp->line);
		free(temp);
	}
	//Free remaining memory
	free(holder);
	free(tail);
	free(buffer);
	fclose(file1);
	//Exit with code 0
	return 0;
}
