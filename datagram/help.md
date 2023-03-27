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


