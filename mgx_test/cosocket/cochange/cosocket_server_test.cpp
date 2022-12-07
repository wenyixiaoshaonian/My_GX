#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include "cosocket.h"
#include <iostream>


void server(void *arg)
{
    struct timeval tv = { 0 };
    for(;;) {
        gettimeofday(&tv, nullptr);
        printf(">>=== time gone : %d us\n",tv.tv_usec);
    }
    

}

int main(int argc, char *argv[])
{
    int i;
    for(i = 0;i<500;i++)
        new Mgx_coroutine(server, nullptr);  // delete by scheduler when coroutine function run finished
    for (;;) {
        Mgx_coroutine_scheduler::get_instance()->schedule();
    }

    return 0;
}
