/* twordcount4.c - threaded word counter for two files.
 *		- Version 4: condition varivale allows counter
 *					functions to report results early
 */

#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>

struct arg_set {
	char *fname;
	int count;
};

struct arg_set *mailbox;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;

int main(int ac,char *av[])
{
	pthread_t t1,t2;				/* two threads*/
	struct arg_set args1,args2;		/* two args*/
	void *count_words(void *);
	int reports_in = 0;
	int total_words = 0;

	if(ac!=3){
		printf("usage: %s file1 file2",av[0]);
		exit(1);
	}
	pthread_mutex_lock(&lock);					/* lock the report box now*/

	args1.fname = av[1];
	args1.count = 0;
	pthread_create(&t1,NULL,count_words,(void *)&args1);

	args2.fname = av[2];
	args2.count = 0;
	pthread_create(&t2,NULL,count_words,(void *)&args2);

	while(reports_in <2){						/* wait two threads*/
		printf("MAIN: waiting for flag to go up \n");
		pthread_cond_wait(&flag,&lock);			/*wait for notify,wait the flag and lock unlock*/
		printf("MAIN: Wow! flag was raised, I have the lock\n");	/*进程取消挂起，读取邮箱中的数据*/
		printf("%7d:%s\n",mailbox->count,mailbox->fname);
		total_words+=mailbox->count;
		if(mailbox == &args1)
			pthread_join(t1,NULL);
		if(mailbox == &args2)
			pthread_join(t2,NULL);
		mailbox = NULL;
		pthread_cond_signal(&flag);
		reports_in++;
	}
}

void *count_words(void *a)
{
	struct arg_set *args = a;		/*cast arg back to correct type*/
	FILE *fp;
	int c,prevc = '\0';

	if((fp = fopen(args->fname,"r"))!= NULL){
		while((c = getc(fp))!= EOF){
			if(!isalnum(c)&&isalnum(prevc))
				args->count++;
			prevc = c;
		}
		fclose(fp);
	}else
		perror(args->fname);
	printf("COUNT: waiting to get lock\n");
	pthread_mutex_lock(&lock);				/*get the mailbox*/
	printf("COUNT: have lock,storing data\n");
	if(mailbox!=NULL)
		pthread_cond_wait(&flag,&lock);			/* the main thread is deal the mailbox*/
	mailbox = args;							/*put ptr to our args there*/
	printf("COUNT: raising flag\n");
	pthread_cond_signal(&flag);				/*raise the flag*/
	printf("COUNT: unlocking box\n");
	pthread_mutex_unlock(&lock);			/*unlock the mailbox*/
	return NULL;

}
