#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include "cosocket.h"
#include <iostream>

#define PORT 8081

struct ser_cli_pair {
    Mgx_cosocket *ser_sock;
    int cli_fd;
};

void server_reader(void *arg)
{
    ser_cli_pair *pair = (ser_cli_pair *)arg;
    Mgx_cosocket *sock = pair->ser_sock;
    int fd = pair->cli_fd;
    int i;
    delete pair;
    for (;;) {
        char buf[1024] = { 0 };
        int ret = sock->recv(fd, buf, 1024, 0);
        if (ret > 0) {
            printf("receive finished\n");
            sock->send(fd, buf, 1024, 0);
        } else if (ret == 0) {
            sock->close(fd);
            break;
        }
    }
}

void server(void *arg)
{
    struct sockaddr_in seraddr;
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = /*inet_addr(INADDR_ANY)*/ htonl(INADDR_ANY);
    seraddr.sin_port = htons(PORT);

    Mgx_cosocket *sock = new Mgx_cosocket();
    sock->socket(AF_INET, SOCK_STREAM, 0);
    sock->bind((struct sockaddr *)&seraddr, sizeof(seraddr));
    sock->listen(500);
    int conn_cnt = 0;

    Mgx_coroutine_scheduler* sch = Mgx_coroutine_scheduler::get_instance();
    long t1 = sch->get_now_ms();
    for (;;) {
        ser_cli_pair *pair = new ser_cli_pair;
        struct sockaddr cliaddr = { 0 };
        socklen_t addrlen  = 0 ;
        int fd = sock->accept(&cliaddr, &addrlen);
        if(fd < 0) {
            printf("create coroutine socket error: %s",strerror(errno));
            continue;
        }
        pair->ser_sock = sock;
        pair->cli_fd = fd;
        new Mgx_coroutine(server_reader, (void *)pair);

    }
}

int main(int argc, char *argv[])
{
#if 1
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
    new Mgx_coroutine(server, nullptr);  // delete by scheduler when coroutine function run finished
    for (;;) {
        Mgx_coroutine_scheduler::get_instance()->schedule();
    }

    return 0;
}
