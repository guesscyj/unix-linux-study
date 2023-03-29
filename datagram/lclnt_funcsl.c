/*
 * lclnt_funcsl.c: functions for the client of the license server
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 * Important variables used throughout
 */

static int pid = -1;						/* Our PID*/
static int sd = -1;							/* Our communications socket*/
static struct sockaddr serv_addr;			/* Server address*/

