#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

//#include "helper.h" 
#include "dispatcher.h"
#include "shell_builtins.h"
#include "parser.h"

/**
 * dispatch_external_command() - run a pipeline of commands
 *
 * @pipeline:   A "struct command" pointer representing one or more
 *              commands chained together in a pipeline.  See the
 *              documentation in parser.h for the layout of this data
 *              structure.  It is also recommended that you use the
 *              "parseview" demo program included in this project to
 *              observe the layout of this structure for a variety of
 *              inputs.
 *
 * Note: this function should not return until all commands in the
 * pipeline have completed their execution.
 *
 * Return: The return status of the last command executed in the
 * pipeline.
 */

//Our functions
void noPipe(struct command *pipeline);
void pipeIn(int piper[2], struct command *pipeline);
void pipeOut(int piper[2], struct command *pipeline);
void midPipe(int piper[2], struct command *pipeline);

static int executeSubProcess(struct command *pipeline, int inPipe[], int usePipe){
	
	int outPipe[2];

	if (pipeline->output_type == 3){//open_pipe
		if (pipe(outPipe) == -1) {
			fprintf(stderr, "Pipe Failed\n");
			return 1;
		}
	}

	if (fork() == 0){

		if (pipeline->input_filename != NULL){//file input
			FILE *fin = fopen(pipeline->input_filename, "r");
			if (!fin){
				fprintf(stderr, "file input failed\n");
				exit (1);
			}
			if (dup2(fileno(fin), 0) == -1){
				fprintf(stderr, "dup failed\n");
				exit (1);
			}
		}

		if (usePipe == 1){//pipeline input
			if (dup2(inPipe[0], STDIN_FILENO) == -1){
				fprintf(stderr, "dup failed\n");
				exit (1);
			}
			close(inPipe[0]);
			close(inPipe[1]);
		}
		
		if (pipeline->output_type == 1){//File overwrite
			FILE * fout = fopen(pipeline->output_filename, "w");
			if (!fout){
				fprintf(stderr, "file output failed\n");
				exit (1);
			}
			if(dup2(fileno(fout), STDOUT_FILENO) == -1){
				fprintf(stderr, "dup failed\n");
				exit (1);
			}
		}

		if (pipeline->output_type == 2){//File Append
			FILE * fout = fopen(pipeline->output_filename, "a");
			if (!fout){
				fprintf(stderr, "file output failed\n");
				exit (1);
			}
			if (dup2(fileno(fout), STDOUT_FILENO) == -1){
				fprintf(stderr, "dup failed\n");
				exit (1);
			}
		}

		if (pipeline->output_type == 3){//pipe out
			if(dup2(outPipe[1], STDOUT_FILENO) == -1){
				fprintf(stderr, "dup failed\n");
				exit (1);
			}
			close(outPipe[0]);
			close(outPipe[1]);
		}

		if(execvp(pipeline->argv[0], pipeline-> argv) == -1){//haha execvp go brrrr
			perror("Error Occurred\n");
			exit(1);
		}
	}
	if(usePipe){//close pipes if needed
		close(inPipe[0]);
		close(inPipe[1]);
	}
	

	int * returnVal = (int *) malloc(sizeof(int));//wait for return
	wait(returnVal);

	if (*returnVal != 0){//error message or something
		fprintf(stderr, "External command failed\n");
		int returnThis = *returnVal;
		free (returnVal);
		return returnThis;
	}

	//free (returnVal);//memory?

	if (pipeline->output_type == 3){//recursion
		return (executeSubProcess(pipeline->pipe_to, outPipe, 1));
	}

	return *returnVal;
}



static int dispatch_external_command(struct command *pipeline)
{

	/*
	 * Note: this is where you'll start implementing the project.
	 *
	 * It's the only function with a "TODO".  However, if you try
	 * and squeeze your entire external command logic into a
	 * single routine with no helper functions, you'll quickly
	 * find your code becomes sloppy and unmaintainable.
	 *
	 * It's up to *you* to structure your software cleanly.  Write
	 * plenty of helper functions, and even start making yourself
	 * new files if you need.
	 *
	 * For D1: you only need to support running a single command
	 * (not a chain of commands in a pipeline), with no input or
	 * output files (output to stdout only).  In other words, you
	 * may live with the assumption that the "input_file" field in
	 * the pipeline struct you are given is NULL, and that
	 * "output_type" will always be COMMAND_OUTPUT_STDOUT.
	 *
	 * For D2: you'll extend this function to support input and
	 * output files, as well as pipeline functionality.
	 *
	 * Good luck!
	 */

	// int wstatus = 0;
	// struct command *temp;
	// int numPipes = 0;
	
	// if(pipeline->pipe_to == NULL){
	// 	noPipe(pipeline);
	// }
	// else{
	// 	temp = pipeline;
	// 	while(temp->pipe_to != NULL){
	// 		numPipes = numPipes + 2;
	// 		temp = temp->pipe_to;
	// 	}
	// 	//Add sizing
   	// 	int* pipefds = calloc(numPipes, sizeof(int));

	// 	//redo?
	// 	for( int i = 0; i < numPipes; i++ ){
	// 		if( pipe(pipefds + i*2) < 0 ){
	// 		perror("Error Occurred: pipeline failed");
	// 		exit(1);
	// 		}
	// 	}

	// 	int commandc = 0;
	// 	temp = pipeline;
	// 	while( commandc < (numPipes/2)+1){
	// 		//fprintf(stderr, "CMD: %s\n", pipeline->argv[0]);
	// 		int pid = fork();
	// 		if( pid == 0 ){
	// 			/* child gets input from the previous command,
	// 				if it's not the first command */

	// 			if( commandc > 0 ){
	// 				// if(pipeline->input_filename != NULL){
	// 				// 	fprintf(stderr, "TODO: filein\n");
	// 				// }
	// 				if( dup2(pipefds[(commandc-1)*2], STDIN_FILENO) < 0){
	// 					perror("Error Occurred: pipeline failed");
	// 					exit(1);
	// 				}
	// 			}
	// 			/* child outputs to next command, if it's not
	// 				the last command */
	// 			if( pipeline->output_type ==  COMMAND_OUTPUT_PIPE){
	// 				//fprintf(stderr, "O:%s\n", pipeline->argv[0]);
	// 				if( dup2(pipefds[commandc*2+1], STDOUT_FILENO) < 0 ){
	// 					perror("Error Occurred: pipeline failed");
	// 					exit(1);
	// 				}
	// 			}
	// 			// else if( pipeline->output_type ==  COMMAND_OUTPUT_FILE_TRUNCATE) {
	// 			// 	fprintf(stderr, "TODO: filetrun");
	// 			// }else if( pipeline->output_type ==  COMMAND_OUTPUT_FILE_APPEND) {
	// 			// 	fprintf(stderr, "TODO: fileapp");
	// 			// }

	// 			for( int j = 0; j < numPipes; j++){
	// 				close(pipefds[j]);
	// 			}

	// 			if(execvp(pipeline->argv[0], pipeline->argv) == -1){
	// 				perror("Error Occurred: execvp: ");
	// 				exit(1); //Exit child since execve failed
	// 			};
	// 		} else if( pid < 0 ){
	// 			perror("Error Occurred: fork ");
	// 			exit(1); //Exit child since execve failed
	// 		}
	// 		else{
	// 			//write ends
	// 			close(pipefds[commandc*2+1]);
	// 			waitpid(pid, NULL, 0);
	// 		}
	// 		commandc++;
	// 		pipeline = pipeline->pipe_to;
			
	// 	}
	
	// }

	// fprintf(stderr, "TODO: status\n");
	// return WEXITSTATUS(wstatus);
	return executeSubProcess(pipeline, NULL, 0);
}


void noPipe(struct command *pipeline){
	if(fork() == 0){
		if(execvp(pipeline->argv[0], pipeline->argv) == -1){
			perror("Error Occurred:");
			exit(1); //Exit child since execve failed
		};
	}else{
		waitpid(-1, NULL, 0);
	}

	return;
}

void pipeIn(int piper[2], struct command *pipeline){
	fprintf(stderr, "In\n");
	if(fork() == 0){
		// if type[0] == r then return 0, otherwise return 1
		dup2(piper[1], 1);
		//close(piper[0]);
		close(piper[1]);
		if(execvp(pipeline->argv[0], pipeline->argv) == -1){
			perror("Error Occurred:");
			exit(1); //Exit child since execve failed
		};
	}else{
		waitpid(-1, NULL, 0);
	}

	return;
}


void pipeOut(int piper[2], struct command *pipeline){
	fprintf(stderr, "Out\n");
	if(fork() == 0){
		// if type[0] == r then return 0, otherwise return 1
		dup2(piper[0], 0);
		//close(piper[1]);
		close(piper[0]);
		if(execvp(pipeline->argv[0], pipeline->argv) == -1){
			perror("Error Occurred:");
			exit(1); //Exit child since execve failed
		};
	}else{
		waitpid(-1, NULL, 0);
	}

	return;

}

void midPipe(int piper[2], struct command *pipeline){
	fprintf(stderr, "Mid\n");
	if(fork() == 0){
		dup2(piper[0], STDIN_FILENO);
		dup2(piper[1], STDOUT_FILENO);
		//close(piper[1]);
		//close(piper[0]);
		if(execvp(pipeline->argv[0], pipeline->argv) == -1){
			perror("Error Occurred:");
			exit(1); //Exit child since execve failed
		};
	}else{
		waitpid(-1, NULL, 0);
	}

	return;
}


/**
 * dispatch_parsed_command() - run a command after it has been parsed
 *
 * @cmd:                The parsed command.
 * @last_rv:            The return code of the previously executed
 *                      command.
 * @shell_should_exit:  Output parameter which is set to true when the
 *                      shell is intended to exit.
 *
 * Return: the return status of the command.
 */
static int dispatch_parsed_command(struct command *cmd, int last_rv,
				   bool *shell_should_exit)
{
	/* First, try to see if it's a builtin. */
	for (size_t i = 0; builtin_commands[i].name; i++) {
		if (!strcmp(builtin_commands[i].name, cmd->argv[0])) {
			/* We found a match!  Run it. */
			return builtin_commands[i].handler(
				(const char *const *)cmd->argv, last_rv,
				shell_should_exit);
		}
	}

	/* Otherwise, it's an external command. */
	return dispatch_external_command(cmd);
}

int shell_command_dispatcher(const char *input, int last_rv,
			     bool *shell_should_exit)
{
	int rv;
	struct command *parse_result;
	enum parse_error parse_error = parse_input(input, &parse_result);

	if (parse_error) {
		fprintf(stderr, "Input parse error: %s\n",
			parse_error_str[parse_error]);
		return -1;
	}

	/* Empty line */
	if (!parse_result)
		return last_rv;

	rv = dispatch_parsed_command(parse_result, last_rv, shell_should_exit);
	free_parse_result(parse_result);
	return rv;
}
