#include <unistd.h>
#include <sys/syscall.h>
#include <cstdlib>
#include <cmath>
#include "mgx_conf.h"
#include "mgx_cosocket.h"


Mgx_conet::Mgx_conet()
{

}

Mgx_conet::~Mgx_conet()
{

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
        mgx_listen_skt->socket   = sock;
        m_listen_skts.push_back(mgx_listen_skt);

        mgx_log(MGX_LOG_INFO, "listen %d succeed", listen_port);
    }
    return true;
}


void Mgx_conet::server(void arg)
{
    pmgx_conn_t c = (pmgx_conn_t)arg;
    for (;;) {
        struct sockaddr cliaddr = { 0 };
        socklen_t addrlen  = 0 ;
        int fd = c->sock->accept(&cliaddr, &addrlen);
        MGX_ASSERT(fd > 0, strerror(errno));
        pmgx_conn_t c_new = get_conn(fd);
        c_new->socket = c->sock;

        new Mgx_coroutine(Mgx_socket::read_request_handler, c_new);
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
    m_epoll_fd = epoll_create(m_worker_conns);
    if (m_epoll_fd < 0) {
        mgx_log(MGX_LOG_STDERR, "epoll_create error: %s", strerror(errno));
        exit(1);
    }

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
        c->socket = (*it)->socket;
        new Mgx_coroutine(server, c);
        
    }
}

/**
 * fd:            socket fd
 * e_type:        event types, eg. EPOLL_CTL_ADD
 * es:            events, eg. EPOLLIN | EPOLLRDHUP
 * mod_action:
 *    only valid if e_type is EPOLL_CTL_MOD,
 *    1 means adding event flag, 0 means removing event flag,
 *    and - 1 means full coverage
 * c:             a connection in connection pool
 */
bool Mgx_conet::epoll_oper_event(int fd, uint32_t e_type, uint32_t es,
                                    EPOLL_ES_MOD_ACTION mod_action, pmgx_conn_t c)
{
    struct epoll_event ev = { 0 };

    switch (e_type) {
        case EPOLL_CTL_ADD:
            c->events = ev.events = es;
            break;
        case EPOLL_CTL_MOD:
            if (mod_action == EPOLL_ES_MOD_ACTION::ADD)
                c->events |= es;
            else if (mod_action == EPOLL_ES_MOD_ACTION::REMOVE)
                c->events &= ~es;
            else if (mod_action == EPOLL_ES_MOD_ACTION::FULL_COVERAGE)
                c->events = es;
            ev.events = c->events;
            break;
        case EPOLL_CTL_DEL:
            break;
        default:
            return false;
    }

    ev.data.ptr = (void *)((uintptr_t)c | c->instance);
    if (epoll_ctl(m_epoll_fd, e_type, fd, &ev) < 0) {
        mgx_log(MGX_LOG_STDERR, "epoll_ctl (%d, %d, %d, %d) error: %s",
                            fd, e_type, es, static_cast<int>(mod_action), strerror(errno));
        return false;
    }
    return true;
}

bool Mgx_conet::epoll_process_events(int timeout)
{
    // int num_events = epoll_wait(m_epoll_fd, m_events, MGX_MAX_EVENTS, timeout);

    // if (num_events < 0) {
    //     mgx_log(MGX_LOG_INFO, "epoll_wait error: %s", strerror(errno));
    //     return errno == EINTR ? true : false;
    // } else if (num_events == 0) {
    //     if (timeout == -1)
    //         return true;

    //     mgx_log(MGX_LOG_ALERT, "epoll_wait error: %s", strerror(errno));
    //     return false;
    // }

    // for (int i = 0; i < num_events; i++) {
    //     pmgx_conn_t c = (pmgx_conn_t) m_events[i].data.ptr;
    //     uintptr_t instance = (uintptr_t)c & 1;
    //     c = (pmgx_conn_t)((uintptr_t)c & (uintptr_t) ~1);

    //     if (c->fd == -1 || c->instance != instance) {
    //         /*
    //          * the stale event from a file descriptor
    //          * that was just closed in this iteration
    //          */
    //         mgx_log(MGX_LOG_DEBUG, "epoll stale event %p", c);
    //         continue;
    //     }

    //     uint32_t recv_events = m_events[i].events;
    //     /*
    //     if (recv_events & (EPOLLERR | EPOLLHUP))
    //          recv_events |= (EPOLLIN | EPOLLOUT);
    //     */

    //     if (recv_events & EPOLLIN) {
    //         c->r_handler(c);
    //     }

    //     if (recv_events & EPOLLOUT) {
    //         c->w_handler(c);
    //     }
    // }
    return true;
}