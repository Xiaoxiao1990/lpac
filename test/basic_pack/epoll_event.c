#include "epoll_event.h"
#include "logs.h"
#include "security_memory.h"
#include "task.h"

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>

#ifndef MAX_EPOLL_SIZE
#define MAX_EPOLL_SIZE					10
#endif


struct event_node_s {
	int listen_fd;
	struct epoll_event event;
	void *epoll_arg;

	//int (*epoll_handle)(void *arg);

	epoll_handle_t epoll_handle;

	struct event_node_s *next;
};

void event_poll_exit(event_t *ep)
{
	ep->exit = 1;
}

static int epoll_event_fd_set(event_t *et, event_node_t *node)
{
	int flags;

	flags = fcntl(node->listen_fd, F_GETFL, 0);

	if (fcntl(node->listen_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		LOGI("Failed to set the listen fd to nonblock mode(errno:%d).", errno);
		return -1;
	}

	if (epoll_ctl(et->epfd, EPOLL_CTL_ADD, node->listen_fd, &(node->event)) < 0) {
		LOGI("Failed to add the fd to epoll(errno:%d).", errno);
		return -1;
	}

	return 0;
}

static int epoll_event_fd_reset(event_t *et, event_node_t *node)
{
	if (epoll_ctl(et->epfd, EPOLL_CTL_DEL, node->listen_fd, &(node->event)) < 0) {
		LOGI("Failed to add the fd to epoll(errno:%d).", errno);
		return -1;
	}

	return 0;
}

int event_register(event_t *et, int fd, uint32_t events, epoll_handle_t callback, void *arg)
{
	event_node_t *temp, *p;

    if (et->fd_size >= MAX_EPOLL_SIZE) {
        LOGI("too many epoll listen fd.");
        return -1;
    }

	p = (event_node_t *)security_zerolloc(sizeof(event_node_t));

	if (NULL == p) {
		LOGI("No memory to allocate event node.");
		return -1;
	}

	p->listen_fd = fd;
	p->event.data.fd = fd;
	p->event.events = events;
	p->epoll_handle = callback;
	p->epoll_arg = arg;
	p->next = NULL;

	if (0 != epoll_event_fd_set(et, p)) {
		security_free(p);
		return -1;
	}

	if (NULL == et->node) {
		et->node = p;
		p->next = NULL;
		//printf("first node\n");
	} else {
		temp = et->node;
		while (temp) {
			//printf("node_addr:%p\n", temp);
			if (NULL == temp->next) {
				temp->next = p;
				p->next = NULL;
				//printf("add node\n");
				break;
			}
			temp = temp->next;
		}
	}

	et->fd_size++;

	return 0;
}

int event_unregister(event_t *et, int fd)
{
	event_node_t *temp, *p;
	int res = -1;
	temp = et->node;

	if (NULL == temp) {
		LOGE("Empty");
		return -1;
	}

	while (temp) {
		if (temp->listen_fd == fd) {
			p = temp;
			if (et->node == temp) {
				// header
				et->node = temp->next;
			}
			epoll_event_fd_reset(et, p);
			security_free(p);
			et->fd_size--;
			res = 0;
		}
		temp = temp->next;
	}

	return res;
}

static void *event_poll_start(void *arg)
{
	event_t *ep = (event_t *)arg;
	struct epoll_event events[MAX_EPOLL_SIZE];
	int nfds;

wait:
	while (ep->fd_size == 0)
		usleep(100000);

	// epoll_wait loop
	while (!ep->exit) {
		nfds = epoll_wait(ep->epfd, events, MAX_EPOLL_SIZE, 500);

		if (nfds == -1) {
			if (errno == EINTR)
				continue;

			LOGI("epoll_wait() error(%d): %s", errno, strerror(errno));
			break;
		} else if (nfds == 0) {
			continue;
		}

		for (int i = 0; i < nfds; i++) {
			event_node_t *p = ep->node;
			for (; p != NULL; p = p->next) {
				if (p->event.data.fd == events[i].data.fd) {
					if (EPOLLIN & events[i].events) {
						p->epoll_handle(p->epoll_arg);
						//p->epoll_handle(p->epoll_arg);
						//event->events = EPOLLIN | EPOLLET | EPOLLONESHOT;
					} else if (EPOLLERR & events[i].events) {
						//TODO
					} else if (EPOLLOUT & events[i].events) {
						//TODO
					}
//                    p->epoll_handle(p->epoll_arg, &events[i]);
//                    if (epoll_ctl(ep->epfd, EPOLL_CTL_MOD, events[i].data.fd, &events[i]) < 0) {
//                        LOGI("Failed to add the fd to epoll(errno:%d).\n", errno);
//                    }
				}
			}
		}

		if (ep->fd_size <= 0)
			goto wait;
	}

	close(ep->epfd);

	pthread_exit(NULL);
}

event_t *event_poll_init(void)
{
	event_t *ep = (event_t *)security_zerolloc(sizeof(event_t));
	ep->epfd = -1;
	ep->exit = 0;
	ep->fd_size = 0;
	ep->node = NULL;

	// create epoll fd
	if ((ep->epfd = epoll_create(MAX_EPOLL_SIZE)) < 0) {
		LOGE("Failed to create the epoll instance.");
		security_free(ep);
		return NULL;
	}

	if (task_maker(event_poll_start, ep) != 0) {
		LOGE("Start uart bus service failure.");
		security_free(ep);
		return NULL;
	}

	return ep;
}
