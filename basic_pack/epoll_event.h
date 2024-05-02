#ifndef __EPOLL_EVENT_H__
#define __EPOLL_EVENT_H__

#include <sys/epoll.h>

typedef int (*epoll_handle_t)(void *arg);

typedef struct event_node_s event_node_t;

typedef struct{
    int epfd;
    int exit;
    int fd_size;
	event_node_t *node;
} event_t;

#ifdef __cplusplus
extern "C"{
#endif

event_t *event_poll_init(void);
int event_register(event_t *et, int fd, uint32_t events, epoll_handle_t callback, void *arg);
int event_unregister(event_t *et, int fd);
void event_poll_exit(event_t *ep);

#ifdef __cplusplus
}
#endif
#endif /* __EPOLL_EVENT_H__ */
