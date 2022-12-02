#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

#define STK_SIZE 100 * 1024
int n = 0,m = 0;
int nr_cpu;
void* ThreadEntry(void* args)
{
    (void) args;
    pid_t tid = syscall(SYS_gettid);
    /* set process with cpu*/
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(tid % nr_cpu, &mask);
    printf("tid %d work with %d cpu \n",tid,tid % nr_cpu);
    sched_setaffinity(0, sizeof(mask), &mask);

    while (1)
    {
        // sleep(1);
    }
}

int main()
{
    pthread_t tid1, tid2, tid3;
    pthread_attr_t attr;
    size_t stacksize;
    int ret;

    pid_t tid = syscall(SYS_gettid);
    /* set process with cpu*/
    nr_cpu  = sysconf(_SC_NPROCESSORS_CONF);
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(tid % nr_cpu, &mask);
    printf("tid %d work with %d cpu \n",tid,tid % nr_cpu);
    sched_setaffinity(0, sizeof(mask), &mask);

    ret = pthread_attr_init(&attr); /*初始化线程属性*/
    if (ret != 0)
        return -1;
#if 0
    ret = pthread_attr_getstacksize(&attr, &stacksize);
    if (ret != 0)
        printf("getstacksize failed...\n");

    printf("before set , pid stack size : %d K\n",stacksize/1024);
    ret = pthread_attr_setstacksize(&attr, STK_SIZE);
    if (ret != 0)
        printf("setstacksize failed...\n");

    ret = pthread_attr_getstacksize(&attr, &stacksize);
    if (ret != 0)
        printf("getstacksize failed...\n");

    printf("before set , pid stack size : %d K\n",stacksize/1024);
#else
    ret = pthread_attr_getstacksize(&attr, &stacksize);
    if (ret != 0)
        printf("getstacksize failed...\n");
    printf("get pid stack size : %d K\n",stacksize/1024);
#endif

#if 0
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
