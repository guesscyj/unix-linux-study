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

### wait的区别

#### 问题1

已知fd = accept(sock,NULL,NULL)会在服务器中调用，accept会阻塞进程

wait为阻塞形调用，waitpid为非阻塞形。当调用wait时，会阻塞当前进程，此时，wait打断系统，accpet将会返回-1,打破循环。

故不应该使用wait,使用waitpid。

#### 问题2

当有多个子进程同时退出时，wait为阻塞形，将会导致第2个信号被阻塞，第3个信号丢失。

#### 解决

waitpid(-1,NULL,WNOHANG)

-1:表示等待的pid，此处表示等待所有的子进程

NULL:第二位用来获取子进程的状态，此处不要

WNOHANG:表示waitpid：没有僵尸进程，则不必等待

## 编写web服务器

通常的功能:

- 列举目录信息ls
- 显示文件内容cat
- 运行程序exec

### web服务器协议

#### HTTP

HTTP全称为HyperText Transfer Protocol

保证客户端与服务器之间的通信

是客户端与服务器之间的请求-响应协议(相当于交互)

客户端和服务器间进行请求是常用方式:

**GET**

**POST**

#### GET

**GET：从服务器请求数据后获取服务端数据**

**常见发起GET请求的方式有：**

> URL 、src/href、表单（form）

**格式:**

> index.php?userName=jack&password=123 
> 语法**（key=value&key=value）：** userName=jack&password=123 叫做查询字符串

**特点：**

> 参数名与参数值之间没有空格
> 参数值不需要使用单双引号包括

具体情况使用GET方法：

1.客户端与服务端的交互像是一个提问(如查询操作、搜索操作、读操作) 

2.请求是为了查找资源，HTML表单数据仅用来帮助搜索

3.请求结果无持续性的副作用（如进行搜索）

4.收集的数据及HTML表单内的输入字段名称的总长不超过1024个字符

5.获取参数或者短小的内容时用GET，如 ?id=5 或者 ?title=abc&url=http...

GET方式提交数据特点：

1、get方式在url后面拼接参数，只能以文本的形式传递参数

2、传递的数据量小，4kb左右（不同浏览器会有差异）

3、安全性低，会将信息显示在地址栏

4、速度快，通常用于对安全性要求不高的请求

#### POST

> **POST：就是发送、提交。向服务器提交/发送要被处理的数据。**

**格式:**

注意⚠️：与GET不同的是，**查询字符串（名称/值）**是在 POST 请求的 HTTP 消息主体中发送的：

> POST /test/demo_form.asp HTTP/1.1
> Host: [http://w3schools.com](https://link.zhihu.com/?target=http%3A//w3schools.com)
> name1=value1&name2=value2

具体情况使用POST方法：

1.交互是一个命令或订单(order)，比提问包含更多信息 

2.交互改变了服务器端的资源并被用户察觉，例如订阅某项服务  

3.用户需要对交互产生的结果负责 

4.请求的结果有持续性的副作用，例如，数据库内添加新的数据行

5.GET方法可能会产生很长的URL，或许会超过某些浏览器与服务器对URL长度的限制，所以使用POST

7.获取大断的内容用POST，通常表单用 POST 方式提交数据，表单的method必须设置为POST

8.对数据有变更的请求（增删）用post；（比如发贴，回复的form 用的是 POST）

**POST方式提交数据特点：**

1、post提交数据相对于get的安全性高一些。（注意：抓包软件也会抓到post的内容，安全性要求高可以进行加密）

2、传递数据量大，请求对数据长度没有要求

3、请求不会被缓存，也不会保留在浏览器的历史记录中

4、用于密码等安全性要求高的场合，提交数据量较大的场合，如上传文件，发布文章等

5、POST方式提交数据上限默认为8M（可以在PHP的配置文件post_max_size选项中修改）

> **举个 ：**
> 访问银行网站或请求带有个人隐私的时候，把账户、密码这些放在URL里被人拦截是不安全的， 所以HTTP设计了POST请求，他可以把请求信息（创建资源，资源的内容）放在HTTP请求里

**总结：**

> GET在浏览器回退时是无害的，而POST会再次提交请求
> GET产生的URL地址可以被Bookmark，而POST不可以
> GET请求会被浏览器主动cache，而POST不会，除非手动设置
> GET请求只能进行url编码，而POST支持多种编码方式
> GET请求参数会被完整保留在浏览器历史记录里，而POST中的参数不会被保留
> GET请求在URL中传送的参数是有长度限制的，而POST没有
> 对参数的数据类型，GET只接受ASCII字符，而POST没有限制
> GET参数通过URL传递，POST放在Request body中
> GET比POST更不安全，因为参数直接暴露在URL上，所以不能用来传递敏感信息

**GET和POST还有一个重大区别，简单的说：**

> GET产生一个TCP数据包；POST产生两个TCP数据包

1. 在网络环境好的情况下，发一次包的时间和发两次包的时间差别基本可以无视

2. 而在网络环境差的情况下，两次包的TCP在验证数据包完整性上，有非常大的优点

3. 并不是所有浏览器都会在POST中发送两次包，Firefox就只发送一次

#### HTTP请求格式

GET /index.html HTTP/1.0			命令 参数 所用协议的版本号

本程序中web中只需要填写参数

