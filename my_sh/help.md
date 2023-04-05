# shell是如何运行的？

1. 执行一个程序
2. 建立一个进程
3. 等待进程结束

## shell的主循环

while(!end_of_input){

​		get command

​		execute command

​		wait for command to finish


## 一个程序运行另一个程序

1. 程序调用execvp
2. 内核从磁盘将程序载入
3. 内核将arglist复制到进程
4. 内核调用main(argc,argv)

## 执行一个新程序program:execvp()

execvp类似替换，调用后会将原程序结束替换执行，故位于execvp后的代码段无法执行。那么shell如何做到等待新指令而不退出?（建立1个新的进程，fork()）

## 建立新进程:process:fork()

fork()调用后内核做：

1. 分配新的内存块和内核数据结构

2. 复制原来的进程到新的进程

3. 向运行进程集添加新的进程

4. 将控制返回给两个进程

    

## 新进程结束:wait()

![wait_pic](./Pic/wait)

pid = wait(&status):

status保存子进程的返回状态,pid为子进程号。exit value记录退出值(exit(x))，signal number 记录信号序列，core dump flag记录发生错误并产生了内核映像



## shell运行方式

![shell_run](./Pic/shell)

fork()创建子进程，exec()运行指定程序，wait()等待新进程结束

## 实现简单shell

![simpleShell](./Pic/simpleShell)

缺陷：

1. 无法正常退出（Ctrl+C生成SIGINT信号,杀死进程)
2. 参数读取



## 进程死亡（exit（））

exit是fork的逆操作，进程调用exit来停止运行。

系统调用_exit()进行以下操作：

1. 关闭所有文件描述符和目录描述符
2. 将该进程的PID置为init进程的PID
3. 如果父进程调用wait或waitpid来等待子进程结束，则通知父进程
4. 向父进程发送SIGCHLD