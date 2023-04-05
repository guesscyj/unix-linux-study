/* hello_single.c - a single thread hello world program*/

#include <stdio.h>
#define NUM 5
void print_msg(char *m);
int main()
{
	print_msg("Hello");
	print_msg("World\n");
}
void print_msg(char *m)
{
	int i;
	for(i=0;i<NUM;i++){
		printf("%s",m);
		fflush(stdout);
		sleep(1);
	}
}
