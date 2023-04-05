//2020051615104陈英杰
#include"ch04.h"
#define flag_a 1//定义a的权限位
#define flag_i 2//定义i的权限位
#define flag_l 4//定义l的权限位
#define flag_err 8//定义error的权限位
#define MAXSIZE 100
DIR *dp;
struct dirent *dir;
struct stat buf;
char str[12];

typedef struct{
	char *_name;//文件名
	ino_t _inode;//文件inode
	time_t _mtime;//文件最后修改时间
	off_t _size;//文件大小
	uid_t _uid;//文件所属用户
	gid_t _gid;//文件所属组
	nlink_t _nlink;//文件硬链接
	int _mode;//文件权限
}include_file;

typedef struct{
	include_file files[MAXSIZE+1];//文件信息
	int length;//文件数组长度
}ls_file;
ls_file L;

//将mode转化为字符形式
void mode_to_letter(int mode,char *str)
{
	str[0]='-';
	if(S_ISDIR(mode))str[0]='d';//文件类型
	if(S_ISCHR(mode))str[0]='c';
	if(S_ISBLK(mode))str[0]='b';

	if(mode & S_IRUSR)str[1]='r';//用户权限
		else str[1]='-';
	if(mode & S_IWUSR)str[2]='w';
		else str[2]='-';
	if(mode & S_IXUSR)str[3]='x';
		else str[3]='-';

	if(mode & S_IRGRP)str[4]='r';//基本组用户的权限
		else str[4]='-';
	if(mode & S_IWGRP)str[5]='w';
		else str[5]='-';
	if(mode & S_IXGRP)str[6]='x';
		else str[6]='-';

	if(mode & S_IROTH)str[7]='r';//其他组
		else str[7]='-';
	if(mode & S_IWOTH)str[8]='w';
		else str[8]='-';
	if(mode & S_IXOTH)str[9]='x';
		else str[9]='-';

	str[10]='\0';
}

int compare(char *s1,char *s2)
{
	int i;
	for(i=0;s1[i]==s2[i]&&s1[i]!=0;i++);
	//循环比较，比较文件名得到排列的顺序
	if(s1[i]==s2[i])return 0;
	if(s1[i]<s2[i])return -1;
	return 1;
}

void ls_sort()//直接插入排序
{
	int j=0,i=0;
	for( i=2;i<=L.length;++i)
		if(compare(L.files[i]._name,L.files[i-1]._name)==-1)
		{
			L.files[0]=L.files[i];
			L.files[i]=L.files[i-1];
			for(j=i-2;compare(L.files[0]._name,L.files[j]._name)==-1;--j)
					L.files[j+1]=L.files[j];
			L.files[j+1]=L.files[0];
		}
}

char *uid_to_name(uid_t uid)//将uid转化为对应char *形
{
	struct passwd *getpwuid(),*pw_ptr;
	static char numstr[10];
	
	if((pw_ptr=getpwuid(uid))==NULL){
		sprintf(numstr,"%d",uid);
		return numstr;
	}
	else
		return getpwuid(uid)->pw_name;
}

char *gid_to_name(gid_t gid)//将gid转化为对应的char *形
{
	struct group *getgrgid(),*grp_ptr;
	static char numstr[10];

	if((grp_ptr=getgrgid(gid))==NULL){
		sprintf(numstr,"%d",gid);
		return numstr;
	}
	else
		return getgrgid(gid)->gr_name;
}
void ls(char *name)//无选项情况
{
	int i=0;
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	while((dir=readdir(dp))!=NULL)
	{
		if(dir->d_name[0]=='.')continue;//不输出隐藏文件
		i++;
		//将对应输出信息存入L结构体中
		L.files[i]._name=dir->d_name;
	}
	L.length=i;//设置L长度
	
	ls_sort();//调用排序函数对其排序
	for(i=1;i<=L.length;i++)
		printf("%s  ",L.files[i]._name);
	closedir(dp);
}

void ls_a(char *name)//参数只有-a情况
{
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	int i=0;
	while((dir=readdir(dp))!=NULL)//输出目录下所有文件
	{
		i++;
		L.files[i]._name=dir->d_name;
	}
	L.length=i;
	ls_sort();
	for(i=1;i<=L.length;i++)
		printf("%s ",L.files[i]._name);
	closedir(dp);
}

void ls_i(char *name)//参数只有-i情况
{
	int i=0;
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	while((dir=readdir(dp))!=NULL)
	{
		if(dir->d_name[0]=='.')continue;//排除隐藏文件
		i++;
		L.files[i]._name=dir->d_name;
		L.files[i]._inode=dir->d_ino;
	}
	L.length=i;//设置数组长度
	ls_sort();//数组排序
	//输出文件信息
	for(i=1;i<=L.length;i++){
		printf("%ld %-5s\t",L.files[i]._inode,L.files[i]._name);
		if(i%5==0)printf("\n");
		}
	closedir(dp);
}

void ls_l(char *name)//参数只有-l情况
{
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	int i=0;
	while((dir=readdir(dp))!=NULL)
	{
		if(dir->d_name[0]=='.')continue;
		stat(dir->d_name,&buf);
		i++;
		L.files[i]._name=dir->d_name;
		L.files[i]._inode=dir->d_ino;
		L.files[i]._nlink=buf.st_nlink;
		L.files[i]._uid=buf.st_uid;
		L.files[i]._gid=buf.st_gid;
		L.files[i]._size=buf.st_size;
		L.files[i]._mtime=buf.st_mtime;
		L.files[i]._mode=buf.st_mode;
	}
	L.length=i;
	ls_sort();
	//closedir(dp);
	for(i=0;i<=L.length;i++)
	{
		mode_to_letter(L.files[i]._mode,str);
		struct tm *t=localtime(&L.files[i]._mtime);//将timer转化为struct tm{}格式，输出对应的时间
		//tm_mon的范围为0-11,tm_mday从1-31,tm_hour从0-23,tm_min从0-59
		printf("%s %2d %s %s %10d %3d月 %2d %02d:%02d %-20s\n",str,L.files[i]._nlink,uid_to_name(L.files[i]._uid),gid_to_name(L.files[i]._gid),L.files[i]._size,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,L.files[i]._name);
	}
	closedir(dp);
}

void ls_ai(char *name)//参数为-a -i
{
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	int i=0;
	while((dir=readdir(dp))!=NULL)
	{
		i++;
		L.files[i]._name=dir->d_name;
		L.files[i]._inode=dir->d_ino;
	}
	L.length=i;
	ls_sort();
	for(i=1;i<=L.length;i++)
		printf("%d %s \t",L.files[i]._inode,L.files[i]._name);
	closedir(dp);
}

void ls_al(char *name)//参数为-a -l
{
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	int i=0;
	while((dir=readdir(dp))!=NULL){
		stat(dir->d_name,&buf);
		i++;
		L.files[i]._name=dir->d_name;
		L.files[i]._inode=dir->d_ino;
		L.files[i]._nlink=buf.st_nlink;
		L.files[i]._uid=buf.st_uid;
		L.files[i]._gid=buf.st_gid;
		L.files[i]._size=buf.st_size;
		L.files[i]._mtime=buf.st_mtime;
		L.files[i]._mode=buf.st_mode;
	}
	L.length=i;
	ls_sort();
	for(i=1;i<=L.length;i++){
		mode_to_letter(L.files[i]._mode,str);
		struct tm *t=localtime(&L.files[i]._mtime);
		printf("%s %-2d %s %s %10d %3d月 %2d %02d:%02d %-20s\n",str,L.files[i]._nlink,uid_to_name(L.files[i]._uid),gid_to_name(L.files[i]._gid),L.files[i]._size,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,L.files[i]._name);
	}
	closedir(dp);
}

void ls_il(char *name)//参数为-i -l
{
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	int i=0;
	while((dir=readdir(dp))!=NULL)
	{
		if(dir->d_name[0]=='.')continue;
		stat(dir->d_name,&buf);
		i++;
		L.files[i]._name=dir->d_name;
		L.files[i]._inode=dir->d_ino;
		L.files[i]._nlink=buf.st_nlink;
		L.files[i]._uid=buf.st_uid;
		L.files[i]._gid=buf.st_gid;
		L.files[i]._size=buf.st_size;
		L.files[i]._mtime=buf.st_mtime;
		L.files[i]._mode=buf.st_mode;
	}
	L.length=i;
	ls_sort();
	for(i=1;i<=L.length;i++)
	{
		mode_to_letter(L.files[i]._mode,str);
		struct tm *t=localtime(&L.files[i]._mtime);//将timer转化为struct tm{}格式，输出对应的时间
		//tm_mon的范围为0-11,tm_mday从1-31,tm_hour从0-23,tm_min从0-59
		printf("%d %s %2d %s %s %10d %3d月 %2d %02d:%02d %-20s\n",L.files[i]._inode,str,L.files[i]._nlink,uid_to_name(L.files[i]._uid),gid_to_name(L.files[i]._gid),L.files[i]._size,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,L.files[i]._name);
	}
	closedir(dp);
}

void ls_ail(char *name)//参数为-a -i -l
{
	if((dp=opendir(name))==NULL)
	{
		err_exit("my_ls无法访问,无法找到目录文件");
		return;
	}
	int i=0;
	while((dir=readdir(dp))!=NULL)
	{
		stat(dir->d_name,&buf);
		i++;
		L.files[i]._name=dir->d_name;
		L.files[i]._inode=dir->d_ino;
		L.files[i]._nlink=buf.st_nlink;
		L.files[i]._uid=buf.st_uid;
		L.files[i]._gid=buf.st_gid;
		L.files[i]._size=buf.st_size;
		L.files[i]._mtime=buf.st_mtime;
		L.files[i]._mode=buf.st_mode;
	}
	L.length=i;
	ls_sort();
	for(i=1;i<=L.length;i++)
	{
		mode_to_letter(L.files[i]._mode,str);
		struct tm *t=localtime(&L.files[i]._mtime);//将timer转化为struct tm{}格式，输出对应的时间
		//tm_mon的范围为0-11,tm_mday从1-31,tm_hour从0-23,tm_min从0-59
		printf("%d %s %2d %s %s %10d %3d月 %2d %02d:%02d %-20s\n",L.files[i]._inode,str,L.files[i]._nlink,uid_to_name(L.files[i]._uid),gid_to_name(L.files[i]._gid),L.files[i]._size,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,L.files[i]._name);
	}
	closedir(dp);
}
int main(int argc,char *argv[])
{
	int c,index,flags=0;
	//将指令进行重排，选出选项参数和非选项参数
	while((c=getopt(argc,argv,":ail"))!=-1)
	{
		switch(c)
		{
			case 'a'://-a
				flags=flags|flag_a;
				break;
			case 'i'://-i
				flags=flags|flag_i;
				break;
			case 'l'://-l
				flags=flags|flag_l;
				break;
			case '?':
				printf("Unknown option '-%c'\n",optopt);
				flags=flags|flag_err;
				break;
			case ':':
				printf("'-%c'missing option arg.\n",optopt);
				flags=flags|flag_err;
				break;
		}
	}
	index=optind;
	if(index==argc)//无地址的情况
	{
		switch(flags){
			case 0:ls(".");break;
			case 1:ls_a(".");break;
			case 2:ls_i(".");break;
			case 3:ls_ai(".");break;
			case 4:ls_l(".");break;
			case 5:ls_al(".");break;
			case 6:ls_il(".");break;
			case 7:ls_ail(".");break;
		}
	}
	for(index;index<argc;index++)//有地址的情况
	{
		switch(flags){
			case 0:ls(argv[index]);break;
			case 1:ls_a(argv[index]);break;
			case 2:ls_i(argv[index]);break;
			case 3:ls_ai(argv[index]);break;
			case 4:ls_l(argv[index]);break;
			case 5:ls_al(argv[index]);break;
			case 6:ls_il(argv[index]);break;
			case 7:ls_ail(argv[index]);break;
		}
	}		
}
