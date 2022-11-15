#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include "mgx_conf.h"
#include "mgx_cosocket.h"
#include "mgx_conet.h"

Mgx_conet::Mgx_conet()
{

}

Mgx_conet::~Mgx_conet()
{

}

bool Mgx_conet::init()
{
    read_conf();
    return Mgx_conet::open_listen_skts();
}

bool Mgx_conet::open_listen_skts()
{
    Mgx_conf *mgx_conf = Mgx_conf::get_instance();
    struct sockaddr_in seraddr;
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = /*inet_addr(INADDR_ANY)*/ htonl(INADDR_ANY);

    char buf[1024] = { 0 };
    for (int i = 0; i < m_listen_cnt; i++) {
        /* cosocket */
        Mgx_cosocket *sock = new Mgx_cosocket();
        sock->socket(AF_INET, SOCK_STREAM, 0);

        sprintf(buf, CONFIG_ListenPort"%d", i);
        int listen_port = mgx_conf->get_int(buf, DEFAULT_LISTEN_PORT);  /* get listen port*/
        seraddr.sin_port = htons(listen_port);
        sock->bind((struct sockaddr *)&seraddr, sizeof(seraddr));
        sock->listen(listen_port);

        /* push mgx_listen_skt to m_listen_skts */
        pmgx_listen_skt_t mgx_listen_skt = new mgx_listen_skt_t;
        mgx_listen_skt->port = listen_port;
        mgx_listen_skt->fd   = sock->m_sockfd;
        mgx_listen_skt->sock   = sock;
        m_listen_skts.push_back(mgx_listen_skt);

        mgx_log(MGX_LOG_INFO, "coroutine listen %d succeed ", listen_port);
    }
    return true;
}


void Mgx_conet::server(void *c)
{
    pmgx_conn_t conn = (pmgx_conn_t)c;
    for (;;) {
        struct sockaddr cliaddr = { 0 };
        socklen_t addrlen  = 0 ;
        int fd = conn->sock->accept(&cliaddr, &addrlen);
        MGX_ASSERT(fd > 0, strerror(errno));
        pmgx_conn_t c_new = get_conn(fd);
        c_new->sock = conn->sock;

        new Mgx_coroutine(std::bind(&Mgx_conet::read_request_handler,this,(void *)c_new), (void *)c_new);
    }
}

void Mgx_conet::scheduler_init()
{
    for (;;) {
        Mgx_coroutine_scheduler::get_instance()->schedule();
    }

}

void Mgx_conet::epoll_init()
{
    conn_pool_init();

    send_msg_th_init();

#ifndef USE_HTTP
    if (m_enabled_heartbeat)
        heart_timer_init();
#endif

    for (auto it = m_listen_skts.begin(); it != m_listen_skts.end(); it++) {
        pmgx_conn_t c = get_conn((*it)->fd);
        if (!c) {
            mgx_log(MGX_LOG_STDERR, "get_conn fd:%d error", (*it)->fd);
            exit(1);
        }
        c->listen_skt = *it;
        (*it)->pconn = c;
        c->sock = (*it)->sock;
        new Mgx_coroutine(std::bind(&Mgx_conet::server, this,(void *)c), (void *)c);
    }
}