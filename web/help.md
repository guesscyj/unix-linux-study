# 连接和协议：编写web服务器

概念与技巧

- 服务器端socket:目的和构造
- 客户端socket:目的和构造
- 客户/服务器协议
- 服务器设计:使用fork来接收多个请求
- 僵尸(zombie)问题
- HTTP

##  服务器设计重点

3个主要操作：

1. 服务器设立服务
2. 客户连接到服务器
3. 服务器和客户处理事物

## 建立连接

服务器：

1. 创建一个socket: socket = socket(PF_INET,SOCK_STREAM,0)
2. 给socket绑定一个地址: bind(sock,&addr,sizeof(addr))
3. 监听接入请求: listen(sock,queue_size)

客户:

1. 创建一个socket: socket = socket(PF_INET,SOCK_STREAM,0)
2. 使用该socket连接服务器: connect(sock,&serv_addr,sizeof(serv_addr))

## 客户/服务器的会话

1.DIY:服务器接收到请求自己处理

2.代理：服务器接收请求，创建一个新进程来处理工作

### 优缺点比较：

1.DIY用于快速简单的任务

2.代理用于慢速的更加复杂的人物

3.使用SIGCHLD来阻止僵尸进程问题

### 僵尸进程

定义：子进程比父进程先结束，而父进程没释放子进程资源

危害：PID占用，资源浪费，占用不必要资源

解决方法：signal(SIGCHLD,child_waiter)，但子进程结束时，会发出SIGCHILD信号，默认情况下会无视，通过调用child_waiter解决

关于child_waiter：可使用wait和waitpid来处理子进程资源问题

