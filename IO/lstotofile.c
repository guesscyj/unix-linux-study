/* whotofile.c
 * purpose: show how to redirect output for another program
 *		idea; fork, then in the child, redirect output, then exec
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main()
{
	int pid;
	int fd;
	printf("About to run who into a file\n");

	/*create a new process or quit */
	if((pid = fork())== -1){
		perror("fork");
		exit(1);
	}
	/* child does the work */
	if(pid == 0){
		close(1);									//close
		fd = open("userlist",O_WRONLY|O_APPEND);				//then open ,open like creat(), open() 
		execlp("ls","ls",NULL);					//and run
		perror("execlp");
		exit(1);
	}
	if(pid!=0){
		wait(NULL);
		printf("Done running who.results in userlist\n");
	}
}
