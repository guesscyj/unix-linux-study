# I/0重定向和管道

**概念和技巧**

- I/O重定向：概念与原因
- 标准输入、输出和标准错误的定义
- 重定向标准I/0到文件
- 使用fork来为其他程序重定向
- 管道（pipe）
- 创建管道后调用fork

**相关的系统调用与函数**

- dup、dup2
- pipe

## 10.1 shell编程

进程间的通信:I/O重定向和管道

ls > my.files

## 10.2 一个shell应用程序：监视系统用户

系统通过一个shell脚本，监控系统用户的登陆和注销

系统用户被分为3个集合，注销，不变，新增

![重定向](./Pic/重定向)

shell脚本体现了三个重要的思路

- shell脚本的功能:与C相比简单易用
- 软件工具的灵活性：每一个工具完成一项特定的、通用的功能
- I/O重定向和管道的使用和作用

## 10.3 标准I/O与重定向的若干概念

I/O重定向基于标准数据流的原理。标准数据流分为

- 标准输入：需要处理的数据流
- 标准输出：结束数据流
- 标准错误输出：错误数据流

### 标准文件描述符

![std](./Pic/std)

所有的Unix工具都使用文件描述符0、1和2

标准输入文件描述符：0

标准输出文件描述符：1

标准错误输出文件描述符：2

### I/0重定向

who > userlist	将stdout连接到一个文件

sort < data		 将stdin连接到一个文件

who | sort		将stdout连接到stdin

### 最低可用文件描述符(Lowest-Available-fd)

文件描述符？一个数组的索引号。每个进程都有其打开的一组文件。这些打开的文件被保持在一个数字中。文件描述符就是某文件在此数组中的索引

![fd](./Pic/fd)

**进程级别的文件描述符表**：内核为每个进程维护一个文件描述符表，该表记录了文件描述符的相关信息，包括文件描述符、指向打开文件表中记录的指针。


**系统级别的打开文件表**：内核对所有打开文件维护的一个进程共享的打开文件描述表，表中存储了处于打开状态文件的相关信息，包括文件类型、访问权限、文件操作函数(file_operations)等。

**系统级别的 i-node 表**：i-node 结构体记录了文件相关的信息，包括文件长度，文件所在设备，文件物理位置，创建、修改和更新时间等，"ls -i" 命令可以查看文件 i-node 节点

## 10.4 如何将stdin定向到文件

### 方法1： close then open

close:挂断一个文件描述符的连接（close(0):将标准输入与终端设备的连接切断）

open:将一个文件连接到stdin上

### 方法2：open..close..dup..close

open(file):打开stdin将要重定向的文件。返回一个文件描述符

close(0):关闭文件描述符0

dup(fd):将文件描述符fd做复制。此次复制使用最低可用文件描述符号0

close(fd):使用close(fd)来关闭文件的原始连接,只留下文件描述符0的连接

![dup](./Pic/dup)

dup小结：#include<unistd.h>

​		int = dup(oldfd)

​		int= dup2(oldfd,otherfd)

dup执行的是修改指向的操作

oldfd:需要复制的文件描述符

ohterfd复制oldfd后得到的文件描述符

int为返回值，是指向后的文件描述符值

## 10.5 为其他程序重定向I/O: who > userlist

1. fork:创建子进程，将命令委托给子进程执行
2. close:关闭子进程相关的标准文件描述符，子进程此时拥有与父进程相同的文件描述符
3. creat:创建修改的文件，之前关闭的文件描述符为最小的将执行该文件
4. exec: **exec相关函数虽然会替换数据，但文件描述符为进程是属性，故无法改变**

![stdRedirect](./Pic/stdoutRedirect)

![stdRedirect](./Pic/stdinRedirect)



​	实现who >> userlog:  将creat改为open即可。close(1)只是修改当前进程的文件描述符

​	sort < data : close(0)

## 10.6 管道编程

### 管道

pipe:创建管道

#include <unistd.h>

result = pipe(int array[2])

result: -1 for false,0 for success

pipe会为array分配最低的两个文件描述符

array[0]:in

array[1]:out

pipedemo.c

![pipe](./Pic/pipedemo)

管道中的通讯信息需要buf来获取输出到显示屏上write(1,buf,len)

### 共享管道

![sharepipe](./Pic/sharepipe)



write():对应in,输入文件

read():对应out,读出文件

本例中，子进程只写入文件，父进程写入文件，读出文件

switch中为什么没有break：oops处理调用函数中发生了错误的情况，调用exit结束进程，多读注释

### pipe

parent做输入重定向 <，child做输出重定向 >

![pipe](./Pic/pipe)

