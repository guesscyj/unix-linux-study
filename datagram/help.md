# 第13章 基于数据报(Datagram)的编程： 编写许可证服务器

学习目标:

- 基于数据报的编程，数据报socket
- TCP和UDP
- 许可证服务器
- 软件时间戳(Software ticket)
- 设计健壮系统
- 设计分布式系统
- Unix域的socket

## 软件控制

程序的运行需要内存，CPU，和一些系统资源。操作系统关心这些问题，但程序需要关心另外一件事，程序的拥有者的允许

从合法性上讲，需要一个许可证(license)，技术限制和自觉遵守

## 许可证管理

许可证程序运行过程：

1. 用户U运行被许可的程序P
2. 程序P向服务器S请求运行许可
3. 服务器检查当前运行程序P的用户数
4. 如果上限未达到，S给予许可，程序P运行
5. 如果到达上限，S拒绝许可，程序P告诉U稍后再试

有点类似于现在的云玩游戏的排队系统

![](./Pic/software)

程序的创建者要编写所有的程序：应用程序和服务器。两个程序作为一个系统，服务器给予应用程序运行许可和实施许可证条款。如果许可证服务器不在运行，应用程序将得不到许可，不能运行。

## 数据报socket

你也可以叫它UDP

流socket传送数据类似于打电话

数据报socket类似于发邮件，客户不必建立连接，只要向特定的地址发送消息，而服务器进程在该地址接受消息

### 流与数据报的比较

流socket使用的网络协议叫TCP（传输控制协议：Transmission Control Protocol）

数据报socket使用的叫UDP(用户数据报协议：User Datagram Protocol)

![tcp&udp](./Pic/TCPUDP)

数据报的只要部分：目的地址，返回地址，消息

UDP访问模式：socket(PF_INET,SOCK_DGRAM,0)与tcp创建的时候参数不同，且程序中使用sendto和recvfrom，而不调用listen





dgrecv.c:接受数据报（服务器端）

dgsend.c:发送数据报（客户端）

dgram.c:辅助函数（创建socket细节，封装细节）

调用: 

服务器端：./ dgrecv 4444 &

客户端: ./dgsend 127.0.0.1 4444 "test info"

./dgsend ipaddr postnumber 消息

### sendto()

目标:从socket发送消息

头文件：#include <sys/types.h> #include <sys/socket.h>

函数定义： nchars = sendto(int socket,const void *msg,size_t len,int flags,const struct sockaddr *dest,socklen_t dest_len)

​	socket: socket id				msg:发送的字符类型的数组				len:发送的字符数

​	flags: 比特的集合，设置发送属性，0表示普通		dest: 指向远端socket地址的指针		dest_len:地址长度

return: -1 出错 ；nchars:发送的字符数

### recvfrom()

目标：从socket接受消息

头文件：#include <sys/types.h>	#include <sys/socket.h>

函数定义: nchars = recvfrom(int socket,const void *msg,size_t len ,int flags,const strcut sockaddr * sender,socklen_t *sender_len)

socket:socket id			msg:字符类型的数组		len:接收的字符数

flags: 表示接收属性的比特的集合，0表示普通	sender：指向远端socket的地址和指针	sender_len:地址长度

return：-1 出错；nchars：发送的字符数

### 数据报应答

编译和测试: gcc dgrecv.c dgram.c -o dgrecv

​					gcc dgsend.c dgram.c -o desend

数据报从一个socket发送到另一个socket。发送者使用sendto来指定消息、长度和目的地。接收者使用recvfrom接收消息。

## 许可证服务器

![HELO](./Pic/helo)

带用户队列的许可证服务器

do_transaction():负责与建立了连接的socket进行交易

客户：HELO:代表申请票据		GBYE：代表退还票据

服务器:对用户的不同请求进行处理，严格规范了交互请求消息的格式



对服务器进行解析:

TICKET_AVAIL：分配给用户的票据是否可用 flag

MAXUSER:限制申请票据用户

say_hello():申请票据，即相关输出，严格要求格式,如果pid == 0,表示可用票据

say_goodbye():收回票据，严格要求格式



handle_request():由4个字符的命令带一个参数组成，服务器检查命令，调用相关函数，即使命令不合法，调用相应应答，客户会阻塞，直到收到应答



编译命令:

gcc lserv1.c lserv_funcs1.c dgram.c -o lserv1

gcc lclnt1.c lclnt_funcs1.c dgram.c -o lclnt1



## 处理一些问题

### 客户端崩溃

客户端崩溃，客户将不会归还票据

![clientBreak](./Pic/clientBreak)

如果某个进程已经不存在了，服务器可以把该进程从数组中去除，释放其占用票据。

1.收回丢失的票据：调度

使用alarm和signal来周期地调用一个函数

![alarm](./Pic/alarm)

考虑问题：

1）在服务器处理客户请求时，触发SIGALAM信号，收回票据，将对票据签出列表及相关数据结构和共享变量修改

​	故在处理客户申请时，应该关闭alarm



2.收回丢失的票据:编程

服务器如何判断进程已经不存在

1.popen运行ps,查看相关进程pid是否存在

2.向进程发送信号,使用kill(pid,0)。如果进程不存在，内核将不会发送信号，而是返回错误并设置errno为ESRCH



关键代码if((ticket_arrat[i]!=TICKER_AVAIL)&&(kill(ticket_array[i],0)==-1)&&(errno==ESRCH))



### 服务器崩溃

服务器崩溃的后果：签出列表丢失，失去进程持有的票据记录。新客户不可以再运行，因为分发许可证的程序已经不存在

解决方法：重启服务器

![servBreak](./Pic/servBreak)

重启服务器可以使新的客户运行，但有新的问题:

1.重启的服务器票据数组为空；服务器含有新的未被取走的票据列表，而崩溃前可能已经发送了票据，再重启会产生更多票据

2.持有旧的服务器的票据的客户在归还使，会被认为是伪造票据



1.票据验证

票据含有数组编号和pid,服务器检查签出列表，如果为空，则认为是自己先去赋予的，服务器将会把票据加到列表中，但可以会已经被分配给新的客户

或者服务器拒绝表中没有的票据，重新申请

2.协议中增加验证

CLIENT：VALD TICKID

SERVER: GOOD or FAIL invalid ticket

3.客户端增加验证

![validate](./Pic/validate)

### 编译命令

gcc lserv2.c lserv_funcs2.c dgram.c -o lserv2

gcc lclnt2.c lclnt_funcs2.c dgram.c -o lclnt2



## 分布式许可证服务器

服务器和客户在同一台机器上运行，不易发生错误，但在不同的主机上时，可能会发生错误

### 重复的进程ID

进程ID在同一台主机上唯一，但在不同主机上可能相同。可能会使一个进程收到多张票据

### 回收票据

kill(pid,0)能够杀死同一台主机上的进程，但不能跨主机执行

可在每台主机上运行一个服务器，但会带来很多新的问题

### 主机崩溃

客户和服务器都可能会崩溃,不同主机崩溃会导致不同的问题

建立分布式许可证系统来支持,有一下3种方法:

1. 客户端服务器和中央服务器通信(客户与本地服务器交互，本地服务器与中央交互)
2. 每个客户都和中央服务器通信(本地服务器只发给客户)
3. 客户服务器和客户服务器通信(本地与其他本地交互)

## unix域socket

### 文件名作为socket地址

2种连接: 流连接和数据报连接。TCP和UDP

2中socket地址:Internet地址和本地地址

​	Internet地址包括主机ID和端口号。本地地址包括Unix地址，无主机ID和端口号

本地地址只有同一台主机才能发送给他。

### 编译指令、运行

gcc logfiled.c -o logfiled

./logfiled >> visitorlog

gcc logfilec.c -o logfilec

./logfilec 'Nice system.Swell software'