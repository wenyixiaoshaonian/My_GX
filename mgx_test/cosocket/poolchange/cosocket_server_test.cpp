#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "cosocket.h"
#include <pthread.h>

#define PORT 8081



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
    pthread_t tid;
    int i,ret;

    for(i = 0;i<500;i++) {
        ret = pthread_create(&tid, NULL, server, NULL);
        if (ret < 0) {
            printf("pthread_create error, ret=%d\n", ret);
            return -1;
        }
    }

    pthread_join(tid, NULL);
    return 0;
}
