# 第14章	线程机制：并发函数的使用

- 程序的执行路线
- 多线程程序
- 创建及销毁线程
- 使用互斥锁机制保证线程间数据的安全共享
- 使用条件变量同步线程间的数据传输
- 传递多个参数给线程
- pthread_create、pthread_join
- pthread_mutex_lock、pthread_mutex_unlock
- pthread_cond_wait、pthread_cond_signal

## 线程

不间断地跟踪指令执行的路径被称为执行路线(thread of execution)

单线程执行过程:

![single](./Pic/single)

多线程执行过程:

一条执行路线进入main函数。初始的路线新建了一条执行路线来运行print_msg(),初始路线继续执行下一条指令从而新建了另一条路线来对print_msg进行第二次调用

![multi](./Pic/multi)

### pthread_create()

目标：创建一个新的线程

头文件：#include <pthread.h>

函数原型：int pthread_create(pthread_t *thread,pthread_attr_t *attr,void *( * func)(void *),void *arg);

参数: thread:指向pthread_t类型变量的指针			attr:指向pthread_attr_t类型变量的指针,或者为NULL

func:指向新进程所运行函数的指针		arg:传递给func的参数

返回值:0 for success；errcode for failed

创建一条新的指向路线，在该路线中调用func(arg)，attr制定路线属性。func是一个函数，它接收一个指针作为它的参数，并且结束后返回一个指针。都被定义为void *,以返回任意类型



### pthread_join()

目标:等待某线程终止

头文件:#include <pthread.h>

函数原型:int pthread_join(pthread_t *thread,void **retval)

参数: thread: 所等待的线程	retval:指向某存储线程返回值的变量

返回值: 0 for success, errcode for failed

pthread_join将会使线程挂起直到thread制定的线程终止。如果retval不是null,线程的返回值就存储在由retval指向的变量中