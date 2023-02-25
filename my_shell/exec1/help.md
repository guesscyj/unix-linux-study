# shell是如何运行的？

## shell的主循环

while(!end_of_input){

​		get command

​		execute command

​		wait for command to finish



![image-20230225203153432](./shellPic)

## 一个程序运行另一个程序

1. 程序调用execvp
2. 内核从磁盘将程序载入
3. 内核将arglist复制到进程
4. 内核调用main(argc,argv)

## execvp的调用

execvp类似与替换，调用后会将原程序结束替换执行，故位于execvp后的代码段无法执行