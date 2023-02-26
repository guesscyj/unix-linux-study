# shell是如何运行的？

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

## execvp的调用

execvp类似与替换，调用后会将原程序结束替换执行，故位于execvp后的代码段无法执行。那么shell如何做到等待新指令而不退出?（建立1个新的进程，fork()）

## 建立新进程

fork()调用后内核做：

1. 分配新的内存块和内核数据结构
2. 复制原来的进程到新的进程
3. 向运行进程集添加新的进程
4. 将控制返回给两个进程
5. 

