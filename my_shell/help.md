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



## smsh4.c version 4-个性化设置

