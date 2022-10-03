#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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

static int dispatch_external_command(struct command *pipeline)
{
	//STORAGE FOR GARBAGE
		//Declare refrences
		// char *comp = "/";
		// char *arg = calloc(strlen(pipeline->argv[0]), sizeof(char)); //TODO:Fix sizeof, returns size of point not of argv
		//TODO: free memory, check mem with shell.debug

		// char* path = getenv("PATH");
		// char* token;
    	// char* rest = path;

		// int tracker = 0;
		// int envSize = 0;
		// while(tracker < strlen(path)){
		// 	if(path[tracker] == ':'){
		// 		envSize++;
		// 	}
		// 	tracker++;
		// }

		// char *env_args[envSize+2];

		// int size = 0;
		// while ((token = strtok_r(rest, ":", &rest))){
		// 	char * temp = calloc(strlen(token), 1);
		// 	temp = strdup(token);
		// 	env_args[size] = temp;
		// 	size++;			
		// }

		// env_args[envSize + 1] = NULL;

		//If starts / run with absolute enviroment
		//TODO:Remove if else
		// if(pipeline->argv[0][0] == comp[0]){
		// 	strncat(arg, pipeline->argv[0], strlen(arg)+strlen(pipeline->argv[0]));
		// }else{
		// 	//Else run with relative enviroment
		// 	//AKA add absolute path to command
		// 	char *bin = "/bin/";
		// 	strncat(arg, bin, strlen(arg)+strlen(bin));
		// 	strncat(arg, pipeline->argv[0], strlen(arg)+strlen(pipeline->argv[0]));
		// }

		//Run command and check for failure

		//fprintf(stderr, "arg:%s\n", arg);
		//fprintf(stderr, ":%s\n", pipeline->argv[0]);

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
	int wstatus = 0;

	if(fork() == 0){
		if(execvp(pipeline->argv[0], pipeline->argv) == -1){
			perror("Error Occurred");
			exit(1); //Exit child since execve failed
		};
	}
	else {
		//Get child process return value
		waitpid(-1, &wstatus, 0);
	}
	
	return WEXITSTATUS(wstatus);
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
