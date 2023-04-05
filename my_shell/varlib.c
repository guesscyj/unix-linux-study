/* varlib.c
 *
 * a simple storage system to store name = value pairs
 * with facility to mark items as part of the environment
 *
 * interface:
 *		VLstore(name,value)		returns 1 for ok, 0 for no
 *		VLlookup(name)			returns string or NULL if not there
 *		VLlist()				prints out current table
 *
 * environment - related functions
 *		VLexport(name)			add name to list of environment vars
 *		VLtable2environ()		copy from table to environ
 *		VLenviron2table()		copy from environ to table
 *
 * details:
 * the table is stored as an array of structs that
 * contain a flag for global and a single string of 
 * the form name = value. This allows EZ addition to the 
 * environment. It makes searching pretty easy,as
 * long as you search for "name = "
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "varlib.h"

#define MAXVARS 200			/*a linked list would be nicer*/

struct var {
		char *str;			/* name = val strings*/
		int global;			/* a boolean*/
};

static struct var tab[MAXVARS];			/*the tables*/

static char *new_string(char*,char*);	/*private methods*/
static struct var* find_item(char *,int);

int VLstore(char *name,char *val)
/*
 * purpose: traverse list, if found, replace it,else add at end
 *			since there is no delete,a blank one is a free one
 *			设置一个环境变量
 * returns: 1 if trouble, 0 if ok(like a command)
 */
{
	struct var *itemp;
	char *s;
	int rv = 1;

	/* find spot to put it and make new string*/
	if((itemp = find_item(name,1))!=NULL&&(s= new_string(name,val))!=NULL)	
	{
		if(itemp->str)		/*has a val?*/
			free(itemp->str);		/*remove if*/
		itemp->str = s;
		rv = 0;
	}
	return rv;
}

char* new_string(char *name,char *val)
/*
 * returns new string of form name = value or NULL on error
 * 为环境变量分配空间
 */
{
	char *retval;
	
	retval = malloc(strlen(name)+strlen(val)+2);		
	if(retval != NULL)
		sprintf(retval,"%s= %s",name,val);
	return retval;
}

char *VLlookup(char *name)	
/*
 * returns value of var or empty string if not there
 *	purpose:查询
 *	returns: if yes return value, not return ""
 */
{
	struct var *itemp;
	if((itemp = find_item(name,0))!=NULL)
		return itemp->str + 1 +strlen(name);
	return "";
}

int VLexport(char *name)
/*
 * marks a var for export, adds it if not there
 * returns 1 if no, 0 if ok
 */
{
	 struct var * itemp;
	 int rv = 1;

	 if((itemp = find_item(name,0))!=NULL){
		itemp ->global = 1;
		rv = 0;
	 }
	 else if(VLstore(name,"")==1)
		rv = VLexport(name);							/*为什么重复调用？*/
														/*调用VLstore可能出现分配内存错误，为防止出现类似错误，重复调用*/
	 return rv;
}

static struct var* find_item(char *name,int first_blank)
/*
 * purpose: searches table for an item
 * returns: return ptr to struct or NULL if not found
 *			or if (first_blank) then ptr to first blank one
 */
{
	int i;
	int len = strlen(name);
	char *s;

	for(i = 0;i<MAXVARS&&tab[i].str !=NULL;i++)
	{
		s = tab[i].str;
		if(strncmp(s,name,len)==0&&s[len]=='='){
			return &tab[i];
		}
	}	
	if(i < MAXVARS && first_blank)
		return &tab[i];
	return NULL;
}

void VLlist()
/*
 * performs the shells set command
 * Lists the contents of the variable table, marking each
 * exported variable with the sysbol '*'
 */
{
	int i;
	for(i = 0;i<MAXVARS && tab[i].str != NULL;i++)
	{
		if(tab[i].global)
			printf(" * %s\n",tab[i].str);
		else 
			printf(" %s\n",tab[i].str);
	}
}

int VLenviron2table(char *env[])
/*
 * purpose:initialize the variable table by loading array of strings
 * returns: 1 if ok, 0 if no
 */
{
	int i;
	char *newstring;
	
	for(i = 0;env[i]!=NULL;i++)
	{
		if(i == MAXVARS)
			return 0;
		newstring = malloc(1+strlen(env[i]));
		if(newstring == NULL)
			return 0;
		strcpy(newstring,env[i]);
		tab[i].str = newstring;
		tab[i].global = 1;
	}
	while(i<MAXVARS){	
		tab[i].str = NULL;
		tab[i++].global = 0;		/*static globals are nulled by default,but init*/
	}
	return 1;
}

char **VLtable2environ()
/*
 * build an array of pointers suitable for making a new environment
 * note, you need to free() this when done to avoid memory leaks
 */
{
	int i,				/*index*/
		j,				/*another index*/
		n = 0;			/* counter*/
	char **envtab;		/*array of pointers*/

	/*first, count the number of global variables*/

	for(i = 0; i<MAXVARS &&tab[i].str!=NULL;i++)
		if(tab[i].global == 1)
			n++;
	
	/* then, allocate space for that many variables */
	envtab = (char**)malloc((n+1)*sizeof(char*));
	if(envtab == NULL)
		return NULL;

	/*then, load the array with pointers*/
	for(i=0,j=0;i<MAXVARS&&tab[i].str!=NULL;i++)
		if(tab[i].global == 1)
			envtab[j++] = tab[i].str;
	envtab[j] = NULL;
	return envtab;
}
