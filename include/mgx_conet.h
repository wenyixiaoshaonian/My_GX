#if !defined(__MGX_CONET_H__)
#define __MGX_CONET_H__

#include <vector>
#include <queue>
#include <list>
#include <map>
#include <unordered_set>
#include <atomic>
#include <cstdint>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstring>
#include "mgx_comm.h"
#include "mgx_thread.h"
#include "mgx_mysql.h"
#include "mgx_redis.h"
#include "mgx_cosocket.h"
#include "mgx_socket.h"

class Mgx_conet : public Mgx_socket
{
public:
    Mgx_conet();
    virtual ~Mgx_conet();

public:
    virtual bool init();
    void epoll_init();
    /* mgx_cosocket_conet.cpp */
    void scheduler_init();
    void server(void *c);

protected:
    bool open_listen_skts();
};

#endif  // __MGX_CONET_H__
