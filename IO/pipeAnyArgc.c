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
		pid=0;					/*and the pid*/
	int pipes[BUFSIZ][2];
	for(int i=0;i+2<ac;i++){
		if(pipe(pipes[i])==-1)
			oops("Pipes open failed",1);
	}
	for(int i=0;i+2<ac;i++){
		if((pid=fork())>0){
			close(pipes[i][1]);
			if(dup2(pipes[i][0],0)==-1)
				oops("could not redirect stdin",2);
			close(pipes[i][0]);
			execlp(av[ac-i-1],av[ac-i-1],NULL);
			oops(av[ac-i-1],4);
			break;
		}else {
			close(pipes[i][0]);
			if(dup2(pipes[i][1],1)==-1)
				oops("could not redirect stdout",3);
			close(pipes[i][1]);
		}
	}
	if(pid==0){
		execlp(av[1],av[1],NULL);
	}
}
