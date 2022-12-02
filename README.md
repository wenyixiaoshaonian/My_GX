[中文](./README_CN.md)

## **A high performance network framework using cosocket, connections pool and threads pool (support tcp and http)**

The framework doesn't depend on any third party components(expend mysql\redis)

### The framework mainly implements the following features

- a master process and multiple subprocesses(worker processes)
    - the master process is responsible for signal processing
        - such as reviving the subprocess, log rotate (`kill -USER1 master_pid`) ...
    - the worker processes handle specific network requests (use epoll LT mode)
- configuration file and log system
- threads pool processing client messages
- a thread and message queues handle the sending of messages
- delay reclaiming connections in connection pool
- heartbeat packet mechanism and detection timer queue to reclain connections
- using specific message format to solve tcp packet sticking problem
- supporting mysql/redis database



By default, the source code is configured to **http** mode. If you need to configure tcp mode, please modify the variable of **config.mk** under the root path.

```
export USE_HTTP = false
```

- Implementation of tcp mode in subclass [Mgx_logic_socket](./bussiness/mgx_logic_socket.cpp)
- Implementation of http mode in subclass [Mgx_http_socket](./http/mgx_http_socket.cpp)

### Quick Start

start in host machine

```bash
make -j4
./mgx
curl 127.0.0.1:8081  # or view web page in browser
```

start in docker
```bash
docker build -t mgx:latest .
docker run -itd -p 8081:8081 mgx:latest
curl 127.0.0.1:8081  # or view web page in browser
```

You can access http://127.0.0.1:8081 to view the web page in browser

### The general structure of Mgx
![image](https://user-images.githubusercontent.com/33289788/198247240-d637ec92-ddfe-4dd6-9344-e18ac0f73e4d.png)

![image](https://user-images.githubusercontent.com/33289788/205205675-374eda84-2d4d-4d5b-9884-b244cddd4063.png)


