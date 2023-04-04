/*
 * lserv1.c
 * License server server program version 1
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/errno.h>
#define MSGLEN 128		/*size of our datagram*/
#define RECLAIM_INTERVAL 60

int setup();
void handle_request(char *req,struct sockaddr_in *client,socklen_t addlen);
void narrate(char *msg1,char *msg2,struct sockaddr_in * clientp);

int main(int ac,char *av[])
{
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);
	char buf[MSGLEN];
	int ret;
	int sock;
	unsigned time_left;

	void ticket_reclaim();			/* version 2 addition*/
	sock = setup();
	signal(SIGALRM,ticket_reclaim);		/*run ticket reclaimer*/
	alarm(RECLAIM_INTERVAL);			/*after this delay*/

	while(1){
		addrlen = sizeof(client_addr);
		ret = recvfrom(sock,buf,MSGLEN,0,&client_addr,&addrlen);
		if(ret != -1){
			buf[ret] = '\0';
			narrate("GOT:",buf,&client_addr);
			time_left = alarm(0);
			handle_request(buf,&client_addr,addrlen);
			alarm(time_left);
		}
		else if(errno!= EINTR)
			perror("recvfrom");
	}
}
