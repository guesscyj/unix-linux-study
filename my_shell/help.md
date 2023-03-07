# 第9章 可编程的shell、shell变量和环境：编写自己的shell

## sh的编程特征

1. 变量
2. I/0(用户输入)
3. if...then（控制）
4. environment（环境变量）

## smsh1.c - version 1

![shellLogic](./Pic/smsh1)

1. next_cmd:从输入流读入下一个命令。调用malloc来分配内存以接受任意长度的命令

2. splitline:将一个字符串分解为字符串数组。

3. execute：执行命令

   

解决问题：1.一行命令执行
2.Ctrl+d的正常退出

## smsh2.c - version 2   if语句

### shell中if语句

if语句做条件检测。在shell中，exit(0)表示成功，所有的Unix语句都以exit(0)表示成功。

if语句的条件当有一系列的命令时，**最后一个命令的exit被用来作这个语句块的条件值**

**关键字fi标识if块的结束**

### 增加if后的逻辑

![image-20230228110355817](./Pic/smsh2)

### 解析

#### 编译指令

gcc -o smsh2 smsh2.c splitline.c execute.c process.c controlflow.c

#### 改动地方

1. 修改主程序，将execute执行前交给process.c判断是否可以执行
2. process.c。判断程序是否进入if段和执行非if段的语句
3. controlflow.c是shell中if语句此时的状态判断和反馈接下来的行为，为处理if段
4. process.c类似与control.c的启动判断

#### 思考

1. 如何处理if中多条指令判断，最后一条为判断条件
2. 此程序是否支持if嵌套

## smsh3.c -version 3 环境变量

### shell中环境变量

环境变量3要素：变量名，值，是否为全局

可以用链表，树等如何数据结构实现

struct var {

char *str;

int global;

};

static struct var_tab[MAXVARS]

### 接口

VLstore(char* var,char* val):增加/更新

VLookup(char* var):取得var的值

VList输出列表到stdout

![smsh3](./Pic/smsh3)

### smsh3.c解析

编译指令：gcc smsh3.c splitline.c execute.c process.c controlflow.c builtin.c varlib.c -o smsh3

环境变量设置方法： day=monday	（禁止使用空格将其分开argv[0]）

查看环境变量： set

修改变量局部性：export day

VLstore():注册环境变量

VLexport():修改环境变量局部性

VLlist():输出所有环境变量

vLlookup():查询，调用find_item

new_string():分配环境变量空间

find_item():查询

okname():环境变量合法检查



改动地方：能够申请环境变量，无法访问环境变量（用户)

子程序无法调用环境变量，变量替换



## smsh4.c version 4-个性化设置（环境）

### 环境

环境是每个程序都可以存取的一个字符串数组,使用extern char **environ;即可访问

![environmodel](./Pic/environModel)

execlp():execlp()会从PATH 环境变量所指的目录中查找符合参数file的文件名, 找到后便执行该文件, 然后将第二个以后的参数当做该文件的argv[0]、argv[1]……, 最后一个参数必须用空指针(NULL)作结束。 

environ:environ指针指向的数组是唯一的例外，当内核执行系统调用execve时，它将数组和字符串复制到新的程序的数据空间，而不是直接替换S

![environCopy](./Pic/environCopy)

**子进程不能修改父进程环境,但fork和exec时，整个环境都主动复制了，所以通过环境来传递数据比较方便，快捷**



char* getenv(const char *name):在当前环境变量中查找环境变量name

int setenv(const char *name,const char *value,int overwrite):创建环境变量

int unsetenv(const char *name):删除环境变量



VLenviron2table,VLtable2environ，将环境变量读入table,将environ转换

## 未解决问题

(1). 变量替换

(2).输入/输出重定向