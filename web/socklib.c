/* socklib.c
 *
 * This file contains functions used lots when writing internet
 * client/server programs. The two main functions here are:
 *
 * int make_server_socket(portnum) return a server socket 
 *									or -1 for error
 * int make_server_socket_q(portnum,backlog)
 *
 * int connect_to_server(char* hostname,int portnum)
 *									return a connected socket
 *									or -1 for error
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <stdlib.h>

#define HOSTLEN 256
#define BACKLOG 1

int make_server_socket_q(int,int);

int make_server_socket(int portnum)
{
	return make_server_socket_q(portnum,BACKLOG);	
}

int make_server_socket_q(int portnum,int backlog)
{
	struct sockaddr_in saddr;			/*build our address here */
	struct hostent *hp;					/* this is part of our*/
	char hostname[HOSTLEN];				/*address*/
	int sock_id;						/*the socket*/

	sock_id = socket(PF_INET,SOCK_STREAM,0);
	if(sock_id == -1)
		return -1;

	/* build address and bind to socket*/
	bzero((void *)&saddr,sizeof(saddr));									/*clear out struct*/
	gethostname(hostname,HOSTLEN);											/*where am I*/
	hp = gethostbyname(hostname);											/*get info about host*/
	bcopy((void *)hp->h_addr,(void*)&saddr.sin_addr,hp->h_length);			/*fill in host part*/
	saddr.sin_port = htons(portnum);										//fill in socket port
	saddr.sin_family = AF_INET;												//fill in addr family
	if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr))!=0)
		return -1;
	
	/*arrange for incoming calls*/
	if(listen(sock_id,backlog)!=0)
		return -1;
	return sock_id;
}

int connect_to_server(char *host,int portnum)
{
	int sock;
	struct sockaddr_in servadd;						/*the number to call*/
	struct hostent *hp;								/*used to get number */

	/*Step 1: Get a socket*/

	sock = socket(PF_INET,SOCK_STREAM,0);			/*get a line*/
	if(sock == -1)
		return -1;

	/*Step 2:connect to server*/

	bzero(&servadd,sizeof(servadd));				/*zero the address*/
	hp = gethostbyname(host);						/*lookup host's ip*/
	if(hp == NULL)
		return -1;
	bcopy(hp->h_addr,(struct sockaddr *)&servadd.sin_addr,hp->h_length);
	servadd.sin_port = htons(portnum);				/*fill port number*/
	servadd.sin_family = PF_INET;					/*fill socket type*/
	
	if(connect(sock,(struct sockaddr *)&servadd,sizeof(servadd))!=0)
		return -1;
	return sock;
}
