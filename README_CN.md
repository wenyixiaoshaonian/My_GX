[English](./README.md)

## **使用 epoll, 连接池和线程池实现的高性能网络框架 (支持 tcp 和 http)**

该框架不依赖于任何第三方组件（除了mysql/redis）

### 主要实现了以下几点

- 一个 master 进程和多个 worker 进程模型
    - master 进程负责信号的处理
        - 比如复活子进程, 日志切割(`kill -USER1 master_pid`) ...
    - worker 进程处理具体网络请求 (使用 epoll 的 LT 模式)
- 配置文件和日志系统
- 线程池处理客户端消息
- 线程通过消息队列处理消息的发送
- 连接池采用延迟回收
- 心跳包机制以及通过检测时间队列来回收无效连接
- 使用特定消息格式来解决 tcp 粘包问题


默认下, 代码配置成了 **http** 模式。 如果需要使用 tcp 模式, 请修改根目录下的 **config.mk** 的变量。

```
export USE_HTTP = false
```

- tcp 模式实现在子类 [Mgx_logic_socket](./bussiness/mgx_logic_socket.cpp)
- http 模式实现在子类 [Mgx_http_socket](./http/mgx_http_socket.cpp)

### 快速开始

在主机中运行

```bash
make -j4
./mgx
curl 127.0.0.1:8081  # or view web page in browser
```

使用 docker 运行
```bash
docker build -t mgx:latest .
docker run -itd -p 8081:8081 mgx:latest
curl 127.0.0.1:8081  # or view web page in browser
```

你可以在浏览器中访问 http://127.0.0.1:8081.

### Mgx 的架构简图
![image](https://user-images.githubusercontent.com/33289788/198247693-2be7fa86-91e0-4d95-82db-61504ef4d5f2.png)

![image](https://user-images.githubusercontent.com/33289788/198247721-7cd7afd0-570b-40db-8b63-a9838888b170.png)
