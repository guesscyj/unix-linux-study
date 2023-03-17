# 第11章 连接到近端或远端的进程：服务器与Socket（套接字）

## 程序处理的数据

- 磁盘文件
- 设备
- 管道
- Sockets

（1,2）磁盘/设备文件：用open命令连接，用read和write传递数据

（3）管道：用pipe命令创建，用fork共享，用read和write传递数据

（4）sockets：用sockets、listen、和connect连接，用read和write传递数据

## 11.3 bc:Unix中使用的计算器

![bc_dc](./Pic/bc_dc)

bc_dc可用的思想：

1. 客户/服务器模型
2. 双向通信
3. 永久性服务（协同进程）

### 11.3.1 编写bc：pipe、fork、dup、exec

1. 创建两个管道
2. 创建一个进程来运行dc
3. 在新创建的进程中，重定向标准输入和标准输出到管道，运行exec dc
4. 在父进程中，读取并分析用户的输入，将命令传给dc,dc响应读取，并把响应传给用户

be_dc():修改文件描述符，为dc分配相关的文件描述符（文件描述符为属性，exec后不会改变）

be_bc():修改bc文件描述符，分析数据，传递给dc

### 协同进程

![coprocess](./Pic/coprocess)

UNIX系统过滤程序从标准输入读取数据, 向标准输出写数据. 几个过滤程序通常在shell管道中连接. 当一个过滤程序既产生某个过滤的输入, 又读取该过滤程序的输出时, 它就变成了协同进程(coprocess).

协同进程是一个，而非两个，是从标准输入、标准输出的那个。协同进程类似帮助另一个进程接收标准输入并过滤，处理输出数据并输出到标准输出设备一样。与其他进程共享数据

优点：

1. 信息共享
2. 加快计算
3. 模块化
4. 方便

示例：

1. 两个管道，一个标准输入，一个标准输出

2. fork
3. 父进程从子进程只读，父进程处理后输出子进程

### fdopen()

表头文件：\#include<stdio.h>

定义函数：FILE * fdopen(int fd,  const char * mode);

功能：将文件描述符像文件一样使用，返回一个打开的文件流fdopen(0,"r");使用fgets等读取文件的方式来读出内容

![fopenAndPopen](./Pic/fopenAndPopen)



## 11.4 popen: 让进程看似文件

表头文件：#include <stdio.h>

定义函数：FILE * popen( const char * command,const char * type);

功能：建立管道连到子进程的标准输出设备或标准输入设备，返回一个文件指针，利用此文件指针来读取子进程的输出设备或是写入到子进程的标准输入设备中

- type == "r":调用进程读进 command 的标准输出
- type == "w": 调用进程读进 command 的标准输入

执行一天命令，而像文件一样可以读写

### 访问数据：文件、应用程序接口(api)和服务器

### 1.从文件获取数据

通过读文件来获取数据（be like "who")。文件的信息服务并不完美。客户端程序依赖特定的文件格式和结构体中的特定成员名称。

### 2.从函数获取数据

一个库函数用标准的函数接口来封装数据的格式和位置。

使用库函数的两种方法：

1.程序使用静态连接来包含实际的函数代码。但这些函数有可能包含的并不是正确的文件名或文件格式

2.程序调用共享库中的函数，但这些共享库并不一定安装到所有系统上

### 3.从进程获取数据

创建一个进程到另一个进程的连接。

调用独立程序获取数据的好处有：

服务器程序可以使用任何程序设计语言编写：shell,c,java等

客户端和服务器端程序可以在不同的机器上运行，所要做的只是和不同机器上的一个进程连接

## 11.5 socket:与远端进程相连

管道的缺陷：

1.管道的通信需要使用fork来共享

2.管道的通信只能在同一台主机上运行



### 工作步骤

1. 向内核申请一个socket
2. 绑定地址到socket上，地址包括主机，端口
3. 在socket上，允许接入呼叫并设置队列长度为1
4. 等待/接收呼叫
5. 传输数据
6. 关闭连接

### socket()

socket：创建一个套接字描述符（客户端和服务器端）,打开套接字（还无法读写）

头文件：#include <sys/types.h> #include <sys/socket.h>

函数定义：int socket(int domain,int type,int protocol);

domain:通信域		type：socket类型。SOCK_STREAM类型管道

propotol:协议socket中使用的协议，默认为0，0代表标准协议（内核中网络代码所使用的协议）

返回值：sockid ：成功

作用于服务器端和客户端

**套接字(socket)是一个抽象层，应用程序可以通过它发送或接收数据，可对其像对文件一样打开、读写、关闭等。套接字允许应用程序将I/O插入到网络中，并与网络中的其他应用程序进行通信，类似门票**

---



### bind()

bind:绑定一个地址到socket（数据的接收和发送操作）

头文件：#include <sys/types.h> #include <sys/socket.h>

函数定义： int bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen);

sockfd: socket的fd,其值为socket（）创建成功的返回值		addr:指向包含地址结构的指针

addrlen：地址长度，其值为**sizeof(struct sockaddr)**

返回值： 0 for success , -1 for failed

作用于服务器端和客户端

**bind将一个地址分配sockfd,addr为自己申明，创建，之后填充其具体的主机地址和端口号，最后填写地址族			类型提交门票后，给位置，准备操作**

---



### listen()

listen: 监听socket上的连接

头文件： #include <sys/socket.h>

函数定义： int listen(int sockfd,int backlog);

sockfd: socket的fd，接收请求的socket，将要转换状态的套接字 		backlog：队列长度，最大的请求数量限制

返回值：0 for success, -1 for failed

作用于服务器端

**将主动套接字转化为监听套接字（listening socket）	用户变安保，待定**	

**		**



### accept()

accept:接受socket上的一个连接

头文件：#include <sys/types.h> #include <sys/socket.h>

函数定义：int accept(int listenfd,struct sockaddr *addr,int *addrlen);

listenfd:向服务器的请求到达的描述符，保安		addr:指向呼叫者地址结构的指针

addrlen:指向呼叫者地址结构长度的指针	**sizeof(struct sockaddr)**

返回值： fd for success, -1 for failed （fd为非负连接描述符）

作用于服务器端

**accept函数阻塞当前进程，等待来自客户端的连接请求到达侦听描述符listenfd,然后在addr中填写客户端的套接字地址，并返回一个已连接描述符（connected descriptor),该描述符被用来Unix I/O函数与客户端通信。	**

**监听描述符通常创建一次，存在于服务器的整个生命周期。已连接描述符是客户端和服务器端之间已经建立起来了的连接的一个端点，每次请求时都会创建一次**

![listenfd&connect](./Pic/listenfd&connect)

**		**

### connect

connect:连接到socket,客户端通过调用connect来建立与服务器的连接

头文件：#include <sys/socket.h>

函数定义：int connect(int clientfd,const struct sockaddr *addr,socklen_t addrlen);

clientfd:用于建立连接的socket		addr:指向服务器地址结构的指针

addrlen:结构的长度，sizeof(sockaddr_in)

0 for success, -1 for failed

作用于客户端

**connect函数试图与套接字地址为addr的服务器建立一个因特网连接。connect函数会阻塞，直到连接成功或发生错误。成功：clientfd描述符就准备好读写，并且得到的连接是由套接字对(x:y,addr.sin_addr:addr.sin_port)刻画，x表示客户端的IP地址，y表示临时端口，确定了客户端主机上的客户端进程。**









