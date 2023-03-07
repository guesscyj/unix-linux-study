/* smash1.c small -shell version 1
 * first really useful version after prompting shell
 * this one parses the command line into strings
 * uses fork, exec, wait, and ignores signals
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "smsh.h"
#include "varlib.h"
#define DFL_PROMPT	">"

int main()
{
	char *cmdline,*prompt,**arglist;
	int result,process(char **);
	void setup();
	
	prompt = DFL_PROMPT;
	setup();

	while((cmdline = next_cmd(prompt,stdin))!= NULL)
	{
		if((arglist = splitline(cmdline))!= NULL){
			result = process(arglist);
			freelist(arglist);
		}
		free(cmdline);
	}
	return 0;
}

void setup()
/*
 * purpose: initializa shell
 * returns: nothing, calls fatal() if trouble
 */
{
	extern char **environ;
	VLenviron2table(environ);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
}

void fatal(char *s1,char *s2,int n)
{
	fprintf(stderr,"Error: %s,%s\n",s1,s2);
	exit(n);
}
