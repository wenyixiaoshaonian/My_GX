#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "cosocket.h"
#include <pthread.h>

#define PORT 8081

void *server_reader(void *arg)
{
    int fd = *(int *)arg;
    for (;;) {
        char buf[1024] = { 0 };
        int ret = recv(fd, buf, 1024, 0);
        if (ret > 0) {
            send(fd, buf, 1024, 0);
        } else if (ret == 0) {
            close(fd);
            break;
        }
    }
}

void *server(void *arg)
{
    struct sockaddr_in seraddr;
    pthread_t tid;
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = /*inet_addr(INADDR_ANY)*/ htonl(INADDR_ANY);
    seraddr.sin_port = htons(PORT);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("listen error: %s",strerror(errno));
        exit(0);
    }

    int opt = 1;
    /* set socket SO_REUSEADDR */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("setsockopt SO_REUSEADDR %d error: %s", listenfd, strerror(errno));
        close(listenfd);
        exit(0);
    }

    /* set socket SO_REUSEPORT to solve thundering herd effect */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        printf("setsockopt SO_REUSEPORT %d error: %s", listenfd, strerror(errno));
    }
    
    /* bind */
    if (bind(listenfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        printf("bind %d error: %s", PORT, strerror(errno));
        close(listenfd);
        exit(0);
    }

    /* listen */
    if (listen(listenfd, 500) < 0) {
        printf("listen %d error: %s", PORT, strerror(errno));
        close(listenfd);
        exit(0);
    }
    for (;;) {
        sockaddr clntAddr;
        socklen_t nSize = sizeof(sockaddr_in);
        int fd = accept(listenfd, &clntAddr, &nSize);
        if(fd < 0) {
            printf("create coroutine socket error: %s",strerror(errno));
            continue;
        }
        void *ptr = &fd;
        pthread_create(&tid, NULL, server_reader, (void *)ptr);

    }
}


int main(int argc, char *argv[])
{
#if 0
    /* fork the same number of processes as CPUs */
    int nr_cpu  = sysconf(_SC_NPROCESSORS_CONF);
    printf("nr_cpu = %d \n",nr_cpu);
    for (int i = 0; i < nr_cpu - 1; i++) {
        if(!fork())
            break;
    }


    /* set process affinity with cpu*/
    pid_t tid = syscall(SYS_gettid);
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(tid % nr_cpu, &mask);
    printf("tid : %d \n",tid);
    sched_setaffinity(0, sizeof(mask), &mask);
#endif
    pthread_t tid1;
    pthread_create(&tid1, NULL, server, NULL);

    pthread_join(tid1, NULL);
    return 0;
}
