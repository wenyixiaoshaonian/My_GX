#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include "cosocket.h"
#include <iostream>

#define PORT            8081
#define CONNECT_TIMEOUT 2000
#define TEST_COUNT      10240

void client(void *arg)
{
    Mgx_cosocket sock;
    int ret;
    sock.socket(AF_INET, SOCK_STREAM, 0);
    Mgx_coroutine_scheduler* sch = Mgx_coroutine_scheduler::get_instance();
    long t1,t2;
    struct sockaddr_in seraddr = { 0 };
    seraddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr);
    seraddr.sin_port = htons(PORT);

    ret = sock.connect((const struct sockaddr *)&seraddr, sizeof(seraddr), 0);
    if(ret < 0)
        printf("connect error: %s",strerror(errno));

    const char *str = "hello";
    sock.send(str, strlen(str), 0);
    t1 = sch->get_now_us();
    char buf[1024] = { 0 };
    ret = sock.recv(buf, 1024, 0);
    if (ret > 0) {
        t2 = sch->get_now_us();
        printf("t2 - t1 = %ldus\n",t2 - t1);
    }
    sock.close();
}

int main(void)
{
    for (int i = 0; i < TEST_COUNT; i++) {
        new Mgx_coroutine(client, nullptr);  // delete by scheduler when coroutine function run finished
    }

    for (;;) {
        Mgx_coroutine_scheduler::get_instance()->schedule();
    }



    return 0;
}
