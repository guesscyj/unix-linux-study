/* process.c
 * command processing layer
 *
 * The process(char ** arglist) function is called by the main loop
 * It sits in front of the excute() function. Thie layer handles
 * two main classes of processing;
 * a)built - in functions(e.g. exit(),set,=,read,..)
 * b)control structures(e.g. if,while,for)
 */

#include <stdio.h>
#include "smsh.h"

int is_control_command(char*);
int do_control_command(char**);
int ok_to_execute();
int builtin_command(char**,int*);

int process(char**args)
/*
 * purpose: process uesr command
 * returns: result of processing command
 * details: if a built - in then call appropriate function, if not 
 *			execute()
 * errors: arise from subroutines,handled there
 */
{
	int rv = 0;
	if(args[0] == NULL)
		rv = 0;
	else if(is_control_command(args[0]))
		rv = do_control_command(args);
	else if(ok_to_execute()){
		if(!builtin_command(args,&rv))				/*检查是否与环境变量相关*/
			rv = execute(args);
	}
	return rv;
}


