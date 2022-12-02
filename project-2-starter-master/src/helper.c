#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h" 
#include "dispatcher.h"
#include "shell_builtins.h"
#include "parser.h"

void echo(struct command *pipeline){
   fprintf(stderr, "%s\n", pipeline->argv[1]);
}



