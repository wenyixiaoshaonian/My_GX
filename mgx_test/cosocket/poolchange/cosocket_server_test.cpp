#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "cosocket.h"
#include <pthread.h>

void *server(void *arg)
{
    struct timeval tv = { 0 };
    for(;;) {
        gettimeofday(&tv, nullptr);
        printf(">>=== time gone : %d us\n",tv.tv_usec);
    }
}


int main(int argc, char *argv[])
{
    pthread_t pid;
    int i,ret;
    int nr_cpu;

    pid_t tid = syscall(SYS_gettid);
    /* set process with cpu*/
    nr_cpu  = sysconf(_SC_NPROCESSORS_CONF);
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(tid % nr_cpu, &mask);
    printf("tid %d work with %d cpu \n",tid,tid % nr_cpu);
    sched_setaffinity(0, sizeof(mask), &mask);

    for(i = 0;i<500;i++) {
        ret = pthread_create(&pid, NULL, server, NULL);
        if (ret < 0) {
            printf("pthread_create error, ret=%d\n", ret);
            return -1;
        }
    }

    pthread_join(pid, NULL);
    return 0;
}
