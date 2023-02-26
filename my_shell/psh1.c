#include<stdio.h>
#include<signal.h>
#include<string.h>
#define MAXARGLENGTH 100
#define MAXARGNUM 20

void main()
{   
    char argbuf[MAXARGLENGTH];
    char *arglist[MAXARGNUM-1];
    char * makestring();
    int argnum=0;
    while(argnum<MAXARGNUM)
    {
        printf("Args:[%d]?",argnum);
        if(fgets(argbuf,MAXARGLENGTH,stdin)&&*argbuf!='\n')
        {
            arglist[argnum++] = makestring(argbuf); //把数据存放到参数列表里面
        }else{
            if(argnum>0)
            {
                arglist[argnum] = NULL; //close the set 
                execute(arglist);
                argnum =0;
            }
        }
    }
}

/*execute the value of the arglist*/
int execute(char *arglist[])
{
    execvp(arglist[0],arglist);
    perror("execute error");
    exit(1);
    return 0;
}
/*malloc the virtual memory for the buf*/
char * makestring(char * buf)
{
    char *cp,*malloc();
    buf[strlen(buf)-1] = '\0'; //set the end of the buf 
    cp = malloc(strlen(buf)+1);
    if(cp ==NULL)
    {
        perror("malloc error");
        exit(1);
    }
    strcpy(cp,buf);
    return cp;
}
