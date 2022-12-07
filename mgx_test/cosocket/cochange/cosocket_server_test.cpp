#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include "cosocket.h"
#include <iostream>


void server(void *arg)
{
    struct timeval tv = { 0 };
    Mgx_coroutine_scheduler *sch = Mgx_coroutine_scheduler::get_instance();
    Mgx_coroutine *co = sch->get_current_coroutine();
    for(;;) {
        gettimeofday(&tv, nullptr);
        printf(">>=== time gone : %d us\n",tv.tv_usec);
        co->yield();
    }
}

int main(int argc, char *argv[])
{
    int i;
    Mgx_coroutine *co[500];
    for(i = 0;i<500;i++)
        co[i] = new Mgx_coroutine(server, nullptr);  // delete by scheduler when coroutine function run finished

    for (;;) {
        for(i = 0;i<500;i++) {
            co[i]->resume();
        }
    }

    return 0;
}
