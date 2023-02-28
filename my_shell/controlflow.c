/* controlflow.c
 *
 * "if" processing is done with two state variables
 * if_state and if_result
 */

#include <stdio.h>
#include <string.h>
#include "smsh.h"

enum states	{NEUTRAL,WANT_THEN,THEN_BLOCK};					//非常重要的标识符，NEUTRAL为正常语句或等待if出现;WANT_THEN为执行if语句后等待then关键字出现，THEN_BLOCK为then表示符出现后等待fi关键字出现
enum results {SUCCESS,FALL};								//if语句是否执行成功

static int if_state = NEUTRAL;
static int if_result = SUCCESS;
static int last_stat = 0;

int syn_err(char *);

int ok_to_execute()
/*
 * purpose: determine the shell should execute a command
 * returns: 1 for yes, 0 for no;
 * details: if in THEN_BLOCK and if_result was SUCCESS then yes
 *			if in THEN_BLOCK and if_result was FAIL then no
 *			if in WANT_THEN then syntax error(sh is different)
 */
{
	int rv = 1;		/* default is positive*/
	if(if_state == WANT_THEN){
		syn_err("then expected");
		rv = 0;
	}
	else if(if_state == THEN_BLOCK && if_result == SUCCESS)
		rv = 1;
	else if(if_state == THEN_BLOCK && if_result == FALL)
		rv = 0;
	return rv;
}

int is_control_command(char *s)
/*
 * purpose: boolean to report if the command is a shell control command
 * returns: 0 or 1
 */
{
	return(strcmp(s,"if") == 0 || strcmp(s,"then") == 0||strcmp(s,"fi") == 0);
}

int do_control_command(char ** args)
/*
 * purpose: Process "if","then","fi" - change state or detect error 更改状态和检测错误
 * returns: 0 if ok, -1 for syntax error
 */
{
	char *cmd = args[0];
	int rv = -1;
	if(strcmp(cmd,"if") == 0){
		if(if_state != NEUTRAL)
			rv = syn_err("if unexpected");
		else {
			/*执行if语句，判断是否执行成功*/
			last_stat = process(args+1);
			if_result = (last_stat == 0 ? SUCCESS:FALL);
			if_state = WANT_THEN;		/**/
			rv = 0;
		}
	}
	else if(strcmp(cmd,"then") == 0){
		if(if_state != WANT_THEN)
			rv = syn_err("then unexpected");
		else {
			/*then紧接if出现，因为这里还没有解决一行多条语句*/
			if_state = THEN_BLOCK;
			rv = 0;
		}
	}
	else if(strcmp(cmd,"fi") == 0){
		if(if_state != THEN_BLOCK)
			rv = syn_err("fi unexpected");
		else {
			//等待then语句块中所有语句结束
			if_state = NEUTRAL;
			rv = 0;
		}
	}
	else 
		fatal("internal error processing:",cmd,2);
	return rv;
}

int syn_err(char *msg)
/* purpose: handles syntax errors in control structures
 * details: resets state to NEUTRAL
 * returns: -1 in interactive mode. Should call fatal in scripts
 */
{
	if_state = NEUTRAL ;
	fprintf(stderr,"syntax error:%s\n",msg);
	return -1;
}
