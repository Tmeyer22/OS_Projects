#include <stdio.h>
#include <string.h>
#include <stdlib.h>




struct node{

char* line;

struct node* previous;
struct node* next;

};






int main(int argc, char *argv[])
{

	if(argc < 2  || argc > 3){
		fprintf(stderr, "usage: reverse <input> <output>\n");
		exit(1);
	}
	
	FILE *outputFile;
	if (argc == 3)
	{
		if(strcmp(argv[1], argv[2]) == 0){
			fprintf(stderr, "error: input and output file must differ\n");
			exit(1);
		}

		

		if ((outputFile = fopen(argv[2], "w")) == NULL)
		{
			fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
			exit(1);
		}
		

	}


	FILE *file1;
	if ((file1 = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
		exit(1);
	}




	char* buffer= NULL;
	size_t buffSize = 0;
	ssize_t lineOut;


	struct node* head = malloc(sizeof( struct node));
	head->next = NULL;
	head->previous = NULL;
	head->line = NULL;
	struct node* curLine = head;
	struct node* holder = malloc(sizeof( struct node));
	struct node* tail = malloc(sizeof( struct node));
	tail->line = NULL;

	 while ((lineOut = getline(&buffer, &buffSize, file1)) != -1){

		holder->next = curLine;
		curLine->next = malloc(sizeof*(curLine->next));

		curLine = curLine->next;
		curLine->previous = holder->next;

		curLine->line = strdup(buffer);
		curLine->next = NULL; 
		tail->next = curLine;

	}
	
if (argc == 2)
{
	struct node* cur = tail->next;
	while(cur->previous != NULL){
		printf("%s\n", cur->line );
		cur = cur->previous;
	}
}
else{


struct node* cur = tail->next;
while(cur->previous != NULL){
	fputs(cur->line, outputFile);
	cur = cur->previous;
}
fclose(outputFile);

}



	
	while(head != NULL){
		struct node* temp = head;
		head = head->next;
		free(temp->line);
		free(temp);
	}

	free(holder);
	free(tail);
	free(buffer);
	fclose(file1);
	

	return 0;
}
