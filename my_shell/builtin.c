/* builtin.c
 * contains the switch and the functions for builtin commands
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "smsh.h"
#include "varlib.h"

int assign(char *);
int okname(char *);

int builtin_command(char **args,int *resultp)
/*
 * purpose: run a builtin command
 * returns: 1 if args[0] is builtin, 0 if not
 * details: test args[0] against all known built - ins. Call functions
 */
{
	int rv = 0;
	
	if(strcmp(args[0],"set")==0){				/*strcmp(char*,char*):所以此处传入args[0],输入时应该为day=daytime,如有空格将传入args[1]*/
		VLlist();								/*列出所有环境变量*/
		* resultp = 0;
		 rv = 1;
	}
	else if(strchr(args[0],'=')!=NULL){			/*同上,被规范了输出格式,可改为args[1]限制空格*/
		*resultp = assign(args[0]);				/*注册环境变量*/
		if(*resultp!= -1)
			rv = 1;
	}
	else if(strcmp(args[0],"export")==0){		/*修改环境变量的状态，设定为全局环境变量*/
		if(args[1]!=NULL&&okname(args[1]))
			*resultp = VLexport(args[1]);
		else
			*resultp = 1;
		rv = 1;
	}
	return rv;
}
int assign(char *str)
/* 
 * purpose: execute name = val AND ensure that name is legal
 * returns: -1 for illegal lval, or result fo VLstore
 * warning: modifies the string, but retores it to normal
 */
{
	char *cp;
	int rv;

	cp = strchr(str,'=');		//char *strchr(const char *str,int c):在str中搜寻第一次出现的字符c的位置
	*cp = '\0';
	rv = (okname(str)? VLstore(str,cp+1):-1);
	*cp = '=';
	return rv;
}

int okname(char *str)
/*
 * purpose: determines if a string is a legal variable name
 * 限制环境变量名
 * returns: 0 for no, 1 for yes
 */
{
	char *cp;
	for(cp = str;*cp;cp++){
		if((isdigit(*cp) && cp==str)||!(isalnum(*cp)||*cp == '_'))				//isdigit(int c):检查所传的字符是否为十进制字符
			return 0;															//isalnum(int c):检查所传的字符是否为字母或数字
	}
	return (cp!=str);
}

