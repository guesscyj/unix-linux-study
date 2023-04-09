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

​	一条执行路线进入main函数。初始的路线新建了一条执行路线来运行print_msg(),初始路线继续执行下一条指令从而新建了另一条路线来对print_msg进行第二次调用

![multi](./Pic/multi)

### pthread_create()

目标：创建一个新的线程

头文件：#include <pthread.h>

函数原型：int pthread_create(pthread_t *thread,pthread_attr_t *attr,void *( * func)(void *),void *arg);

参数: thread:指向pthread_t类型变量的指针			attr:指向pthread_attr_t类型变量的指针,或者为NULL

func:指向新进程所运行函数的指针		arg:传递给func的参数

返回值:0 for success；errcode for failed

​	创建一条新的指向路线，在该路线中调用func(arg)，attr制定路线属性。func是一个函数，它接收一个指针作为它的参数，并且结束后返回一个指针。都被定义为void *,以返回任意类型



### pthread_join()

目标:等待某线程终止

头文件:#include <pthread.h>

函数原型:int pthread_join(pthread_t *thread,void **retval)

参数: thread: 所等待的线程	retval:指向某存储线程返回值的变量

返回值: 0 for success, errcode for failed

pthread_join将会使线程挂起直到thread制定的线程终止。如果retval不是null,线程的返回值就存储在由retval指向的变量中



## 线程间的分工合作

3个例子 twordcount1.c twordcount2.c twordcount3.c

### twordcount1.c

两个线程，一个计数器

![twordcount1](./Pic/twordcount1)



​	读文件，计算文件中的单词数量，使用全局变量作为计数器，线程之间无合作来修改同一个变量

​	所有的线程对同一个计数器进行++操作，会出现访问同一个寄存器的问题，可能会有两个线程同时读取出来，放入同一个值，导致计算错误

### twordcount2.c

两个线程，一个计数器，一个互斥量

![twordcount3](./Pic/twordcount2)

​	在twordcount1.c的基础上增加了互斥量 pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER

​	在申请访问寄存器前，等待互斥锁解开再锁住互斥锁，每次进行计数器访问时，都将遵守该操作。如果一个线程不遵守，直接去修改计数器的值的话，程序员也没办法。且等待互斥锁打开增加了线程运行的时间。降低了系统的性能

### twordcount3.c

两个线程，两个计数器，向线程传递多个参数

![twordcount3](./Pic/twordcount3)

​	使用一个结构体作为参数struct arg_set{char *fname,int count};来保存各自的单词数量，传递给pthread_create()参数变为了结构体。两个线程同时计算自己文件的单词数量。

​	传递本地结构体指针的方法避免了对互斥锁的依赖，且消除了全局变量的使用。消耗了内存空间来保证运行时间以及运行的正确性。

### 互斥锁

#### pthread_mutex_lock()

目标： 等待互斥锁解开然后再锁住互斥量

头文件： #include <pthread.h>

函数原型： int pthread_mutex_lock(pthread_mutex_t *mutex)

参数： mutex： 指向互斥锁对象的指针

返回值： 0 for success, errcode for failed

#### pthread_mutex_unlock()

目标：给互斥量解锁

头文件： #include <pthread.h>

函数原型： int pthread_mutex_unlock(pthread_mutex_t *mutex)

参数： mutex： 指向互斥锁对象的指针

返回值：0 for success, errcode for failed

正常情况下，互斥量会支持运行，但如果一个线程试图解锁一个没被锁住的互斥量，会发生死锁，互斥量具有诸多问题，不同线程系统有不同的出理方案。

## 14.4线程与进程

线程与进程比较:

### 1.共享数据空间

​	fork出的进程将复制原有进程的一份数据空间，不同进程对数据空间进行操作，互不影响。

​	thread出的线程，共享着同一个数据空间。故在编程时需考虑malloc、free等问题。往往是不释放空间，导致了空间的囤积。返回静态局部变量的指针的函数无法兼容多线程环境。

### 2.文件描述符

​	fork出的进程，将复制文件描述符，子进程继承了父进程的文件描述符，当子进程的文件描述符关闭时，父进程的仍然打开，互不影响。

​	thread出的线程，会将同一个文件描述符**传递**给多个线程，关闭了一个文件描述符，相当于将所有的线程的文件描述符关闭，共享着文件描述符。

### 3.进程ID

​	fork出的进程，拥有着不同的进程ID。

​	thread出的线程，都共享着同一个进程ID，故当某一个线程执行有关进程的操作时，会影响到所有的线程，如exec、fork、exit等。exec会调用新的程序取代当前进程，当前进程中的所有线程都将被取代。线程fork，其他的线程不会复制给新的进程。

### 信号处理

​	不同的系统线程处理系统不同，此处说一下linux内核处理方式

对于进程下的多个线程来说， 信号处理函数是共享的。

在Linux内核实现中， 同一个线程组里的所有线程都共享一个struct sighand结构体。 该结构体中存在一个action数组， 数组共64项， 每一个成员都是k_sigaction结构体类型， 一个k_sigaction结构体对应一个信号的信号处理函数。

## 14.5 线程间互通消息

线程间没有使用signal机制，而是一个类似于signal的机制。

使用加锁的变量机制来传递数据。线程挂起。线程共享数据。

![thread_singal](./Pic/thread_singal)

### pthread_cond_wait

目标： 使线程挂起，等待某条件变量的信号

头文件： #include <pthread.h>

函数原型： int pthread_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex)；

参数： cond 指向某条件变量的指针		mutex 指向互斥锁对象的指针

返回值：	0 for success,errcode for failed

pthread_cond_wait是线程挂起，直到另一个线程通过条件变量发出消息。pthread_cond_wait函数总是和互斥锁一起使用。先自动释放指定的锁，然后等待条件变量的变化。

### pthread_cond_signal

目标： 唤醒一个正在等待的线程

头文件： #include <pthread.h>

函数原型： int pthread_cond_signal(pthread_cond_t *cond)；

参数: 	cond 指向某条件变量的指针

返回值： 	0 for success,errcode for failed

pthread_cond_signal 会通过条件变量cond发消息。若没有线程等待消息，则不会发生什么，若有多个线程等待，唤醒其中一个。