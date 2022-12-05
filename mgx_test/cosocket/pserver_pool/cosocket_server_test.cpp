#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "cosocket.h"
#include <pthread.h>

#define PORT 8081
struct fd_ctx {
    int fd[10241];
    int numw;
    int numr;
};
int in = 0;
pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  m_cond = PTHREAD_COND_INITIALIZER;

fd_ctx fd_t = {{0},0,0};
void *server_reader(void *arg)
{
    for(;;) {
        int err = pthread_mutex_lock(&m_mutex);   /* lock */
        if (err != 0)
            printf("pthread_mutex_lock error: %s", strerror(err));
        while (fd_t.numr >= fd_t.numw) {
            pthread_cond_wait(&m_cond, &m_mutex);
        }
        
        int fd = fd_t.fd[fd_t.numr++];
        if(fd_t.numr > 10240)
            fd_t.numr = 0;

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
        err = pthread_mutex_unlock(&m_mutex);
        if (err != 0)
            printf("pthread_mutex_unlock error: %s", strerror(err));
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
        fd_t.fd[fd_t.numw] = accept(listenfd, &clntAddr, &nSize);
        if(fd_t.fd[fd_t.numw] < 0) {
            printf("create coroutine socket error: %s",strerror(errno));
            continue;
        }
        fd_t.numw++;
        if(fd_t.numw > 10240)
            fd_t.numw = 0;
        int err = pthread_cond_signal(&m_cond);
        if (err != 0)
            printf("pthread_cond_signal error: %s", strerror(err));
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
    pthread_t tid;
    int i,ret;

    for(i = 0;i<500;i++) {
        ret = pthread_create(&tid, NULL, server_reader, NULL);
        if (ret < 0) {
            printf("pthread_create error, ret=%d\n", ret);
            return -1;
        }
    }
    printf(">>>===111\n");
    ret = pthread_create(&tid, NULL, server, NULL);
    if (ret < 0) {
        printf("pthread_create error, ret=%d\n", ret);
        return -1;
    }
    pthread_join(tid, NULL);
    return 0;
}
