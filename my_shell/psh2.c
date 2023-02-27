/* prompting shell version 2
 *
 * Solves the 'one-shot' problem of version 1
 *		Uses execvp(),but fors()s first so that the
 *		shell waits around to perform anther command
 *	
 *	New problem: shell catches signals. Run vi,press ^C 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define MAXARGS 20
#define ARGLEN	100

int main()
{
	char *arglist[MAXARGS+1];
	int numargs;
	char argbuf[ARGLEN];
	char *makestring();

	numargs = 0;
	while(numargs<MAXARGS)
	{
		printf("ARF[%d]?",numargs);
		if(fgets(argbuf,ARGLEN,stdin)&& *argbuf!='\n') /*编写时此处发生了编程错误，fgets(argbuf)读取换行符时，
														 argbuf='\n'.而非"\n".换行会使fgets停止读取，但仍会保存
														 "\n"会隐式生成一个\0*/
			arglist[numargs++] = makestring(argbuf);
		else 
		{
			if(numargs > 0){
				arglist[numargs] = NULL;
				execute(arglist);
				numargs = 0;
			}
		}
	}
	return 0;
}

int execute(char* arglist[])
{
	/* use fork and execvp and wait to do it*/

	int pid,exitstatus;

	pid = fork();
	switch(pid){
		case -1:
			perror("fork failed");
			exit(1);
		case 0:
			execvp(arglist[0],arglist);
			perror("execvp failed");
			exit(1);
		default:
			while(wait(&exitstatus)!=pid);
			printf("child exited with status %d,%d\n",exitstatus>>8,exitstatus&0377);
	}
}

char *makestring(char* buf)
{
	/* trim off newline and create storage for the string */
	char *cp,*mallloc();
	
	buf[strlen(buf)-1] = '\0'; /*newline is 换行符*/
	cp = malloc(strlen(buf)+1);
	if(cp == NULL){
		fprintf(stderr,"no memory\n");
		exit(1);
	}
	strcpy(cp,buf);
	return cp;
}
