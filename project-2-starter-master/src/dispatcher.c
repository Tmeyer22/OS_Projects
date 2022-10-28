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

//Our function
static int command(struct command *pipeline, int nextPipe[], bool inPipe){
	
	int outPipe[2];
	int childName;

	if (pipeline->output_type == COMMAND_OUTPUT_PIPE){//open_pipe
		if (pipe(outPipe) == -1) {
			fprintf(stderr, "Pipe Failed\n");
			return 1;
		}
	}

	childName = fork();
	if (childName == 0){

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

		if (inPipe == true){//pipeline input
			if (dup2(nextPipe[0], STDIN_FILENO) == -1){
				fprintf(stderr, "dup failed\n");
				exit (1);
			}
			close(nextPipe[0]);
			close(nextPipe[1]);
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
	if(inPipe){//close pipes if needed
		close(nextPipe[0]);
		close(nextPipe[1]);
	}
	

	int returnVal;//wait for return
	waitpid(childName, &returnVal, 0);

	if (pipeline->output_type == 3){//recursion
		return (executeSubProcess(pipeline->pipe_to, outPipe, true));
	}

	if (WIFEXITED(returnVal) == 0 || WEXITSTATUS(returnVal) != 0){//error message or something
		fprintf(stderr, "External command failed\n");
		return 1;
	}

	return 0;
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
	return executeSubProcess(pipeline, NULL, false);
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
