/* pipe.c
 * Demonstrates how to create a pipeline from one process to another
 * * Takes two args, each a command, and connects
 *	 av[1]s output to input of av[2]	 be like "pipe who sort" who输出重定向给sort
 * * usage:pipe command1 command2
 *	 effect:command1 | command2
 * * Limitations: commands do not take arguments 输入不带参数，缺陷一
 * * uses execlp() since known number of args
 * * Note: exchange child and parent and watch fun
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define oops(m,x) {perror(m);exit(x);}

int main(int ac,char**av)
{
	int thepipe[2],				/*two file descriptors*/
		newfd,					/*useful for pipes*/
		pid,					/*and the pid*/
		childpipe[2];			/*参数为3个时，子进程应该fork一个来处理，而非父进程fork*/
	if(ac!=4){
		fprintf(stderr,"usage:pipe cmd1 cmd2\n");
		exit(1);
	}
	if(pipe(thepipe)==-1)	/*get a pipe*/
		oops("Cannot get a pid",1);
	
	/*now we have a pipe, now lets get two process */
	if((pid = fork())==-1)			/*get a proc*/
		oops("Cannot fork",2);

	/*Right Here, there are two process */
	/*				parent will read from pipe	*/

	if(pid>0){				/*parent will exec av[2]*/
		close(thepipe[1]);	/*parent dosen't write to pipe*/

		if(dup2(thepipe[0],0)==-1)
			oops("could not redirect stdin",3);
		close(thepipe[0]);
		execlp(av[3],av[3],NULL);
		oops(av[3],4);
	}

	if(pipe(childpipe)==-1)
		oops("child could not pipe",1);
	if((pid = fork())==-1)
		oops("Child Cannot fork",2);
	if(pid>0){
		/*first child read from childpipe then exec and write into the pipe*/
		close(childpipe[1]);
		if(dup2(childpipe[0],0)==-1)
			oops("could not redirect child stdin",3);
		close(childpipe[0]);

		close(thepipe[0]);		/* child doesn't read from pipe*/
	
		if(dup2(thepipe[1],1)==-1)
			oops("could not redirect stdout",4);
		close(thepipe[1]);
		execlp(av[2],av[2],NULL);
		oops(av[1],5);
	}
	/* child's child exec av[1] and write into childpipe  */
	close(childpipe[0]);

	if(dup2(childpipe[1],1)==-1)
		oops("child's child could not redirect stdout",4);
	close(childpipe[1]);
	execlp(av[1],av[1],NULL);
	oops(av[1],5);
}
