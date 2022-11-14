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
    ssize_t     m_pkg_hdr_size = sizeof(mgx_pkg_hdr_t);
    ssize_t     m_msg_hdr_size = sizeof(mgx_msg_hdr_t);

    virtual bool init();
    virtual void th_msg_process_func(char *buf);
    void epoll_init();
    bool epoll_process_events(int timeout);
    bool epoll_oper_event(int fd, uint32_t e_type, uint32_t flag, EPOLL_ES_MOD_ACTION mod_action, pmgx_conn_t pconn);

protected:
    void send_msg(char *send_buf);
    /* mgx_socket_conn.cpp */
    void insert_recy_conn_queue(pmgx_conn_t c);
    /* mgx_socket_request.cpp */
    ssize_t recv_process(pmgx_conn_t c, char *buf, ssize_t buf_size);

private:
    int m_listen_cnt = 1;
    int m_worker_conns = DEFAULT_WORKER_CONNS;
    std::vector<pmgx_listen_skt_t> m_listen_skts;
    int m_epoll_fd;
    struct epoll_event m_events[MGX_MAX_EVENTS];

    /* connection */
    std::queue<pmgx_conn_t> m_pconns_queue;       /* connection pool */
    std::queue<pmgx_conn_t> m_pconns_free_queue;  /* free connections in connection pool */
    std::unordered_set<pmgx_conn_t> m_recy_conn_set;    /* recovery connections in connection pool */
    std::atomic<int> m_total_conns_cnt;         /* the number of connection pool */
    std::atomic<int> m_free_conns_cnt;          /* the number of free connections */
    std::atomic<int> m_total_recy_conns_cnt;    /* the number of recovery connections */
    int m_recy_conn_wait_time = DEFAULT_RECY_CONN_WAIT_TIME;
    pthread_mutex_t m_conn_mutex;
    pthread_mutex_t m_recy_queue_mutex;
    Mgx_thread *m_recy_thread;

    /* send */
    std::list<char *> m_send_list;
    std::atomic<int> m_send_list_cnt;
    sem_t m_send_queue_sem;
    pthread_mutex_t m_send_queue_mutex;
    Mgx_thread *m_send_thread;

    /* heartbeat */
    int m_enabled_heartbeat;
    int m_heart_wait_time;
    std::multimap<time_t, pmgx_msg_hdr_t> m_timer_queue;
    pthread_mutex_t m_timer_que_mutex;
    std::atomic<int> m_timer_que_size;
    time_t  m_timer_que_head_time;
    Mgx_thread *m_monitor_timer_thread;

    /* mgx_socket.cpp */
    void read_conf();
    bool open_listen_skts();
    void close_listen_skts();
    void set_nonblock(int listenfd);
    void send_msg_th_init();
    void send_msg_th_func();
    ssize_t send_uninterrupt(pmgx_conn_t c, char *buf, ssize_t size);

    /* mgx_socket_conn.cpp */
    void conn_pool_init();
    void conn_pool_destroy();
    pmgx_conn_t get_conn(int sock_fd);
    void free_conn(pmgx_conn_t c);
    void close_conn(pmgx_conn_t c);
    void recy_conn_th_func();

    /* mgx_socket_accept.cpp */
    void event_accept(pmgx_conn_t pconn_listen);

    /* mgx_socket_request.cpp */
    void send_msg_handler(pmgx_conn_t c);
    virtual void _read_request_handler(pmgx_conn_t c);
    void read_request_handler(pmgx_conn_t c);
    void read_request_handler_process_v1(pmgx_conn_t c);  /* process package header */
    void read_request_handler_process_v2(pmgx_conn_t c);  /* process package body */

    /* mgx_socket_time.cpp */
    void heart_timer_init();
    void add_to_timer_queue(pmgx_conn_t pconn);
    time_t get_earliest_time();
    pmgx_msg_hdr_t get_over_time_timer(time_t cur_time);
    void monitor_timer_th_func();
    void delete_from_timer_queue(pmgx_conn_t pconn);

    /* mgx_cosocket_conet.cpp */
    void scheduler_init();
    void server(pmgx_conn_t c);
};

#endif  // __MGX_CONET_H__
