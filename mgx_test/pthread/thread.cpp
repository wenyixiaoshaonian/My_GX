#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define STK_SIZE 100 * 1024
int n = 0,m = 0;
void* ThreadEntry(void* args)
{
    (void) args;
    printf("create pthread successful.... %d\n",n++);
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
#if 1
    int i ;
    for(i = 0;i < 100770 + 1 ;i++) {
        ret = pthread_create(&tid1, &attr, ThreadEntry, NULL);
        if (ret != 0)
		    printf("can't create thread %d    n = %d  m = %d\n",ret,n,m++);
            ;
    }
    pthread_join(tid1,NULL);
#else
    pthread_create(&tid1, &attr, ThreadEntry, NULL);
    pthread_create(&tid2, &attr, ThreadEntry, NULL);
    pthread_create(&tid3, &attr, ThreadEntry, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    return 0;
#endif
}
