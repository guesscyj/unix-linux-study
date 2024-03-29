/* 
 * lclnt1.c
 * License server client version 1
 * link with lclnt_funcs1.o dgram.o
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup();
void shut_down();
void release_ticket();
void do_regular_work();
int get_ticket();

int main(int ac,char *av[])
{
	setup();
	if(get_ticket()!=0)
		exit(0);

	do_regular_work();

	release_ticket();
	shut_down();
}

/*
 * do_regular_work the main work of the application goes here
 */
void do_regular_work()
{
	printf("SuperSleep version 1.0 Running - Licensed Software\n");
	sleep(10);/*our patened sleep algorithm*/
}

