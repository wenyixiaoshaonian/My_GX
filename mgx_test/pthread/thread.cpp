#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define STK_SIZE 100 * 1024
void* ThreadEntry(void* args)
{
    (void) args;
    while (1)
    {
        sleep(1);
    }
}

int main()
{
    pthread_t tid1, tid2, tid3;
    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr); /*初始化线程属性*/
    if (ret != 0)
        return -1;
#if 0
    ret = pthread_attr_setstacksize(&attr, STK_SIZE);
    if(ret != 0)
        return -1;
#endif

    pthread_create(&tid1, &attr, ThreadEntry, NULL);
    pthread_create(&tid2, &attr, ThreadEntry, NULL);
    pthread_create(&tid3, &attr, ThreadEntry, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    return 0;
}
