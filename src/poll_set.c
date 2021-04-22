#include "../include/net.h"

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include <sys/epoll.h>

struct poll_set* poll_set_new(int max_events, poll_set_callback_func* data_callback, poll_set_callback_func* close_callback) {
	void* p = malloc(sizeof(struct poll_set) + sizeof(struct epoll_event)*max_events);
	if (p == NULL) {
		printf("[error] malloc(...): failed\n");
		return NULL;
	}

	struct poll_set* poll_set = p;
	poll_set->fd = epoll_create1(0);
	if (poll_set->fd == -1) {
		printf("[error] epoll_create1(...): failed with error %s\n", strerror(errno));
	}
	poll_set->max_events = max_events;
	poll_set->events = (struct epoll_event*) (poll_set+1);
	poll_set->data_callback = data_callback;
	poll_set->close_callback = close_callback;

	return poll_set;
}

void poll_set_free(struct poll_set* poll_set) {
	// Is there no way to close a poll set? Is it not necessary?
	//close(poll_set->fd); // Is this necessary?
	free(poll_set);
}

void poll_set_add(struct poll_set* poll_set, int fd) {
	struct epoll_event ev = {
		.data.fd = fd,
		.events = EPOLLIN | EPOLLRDHUP,
	};
	if (epoll_ctl(poll_set->fd, EPOLL_CTL_ADD, fd, &ev)) {
		printf("[error] poll_set_add(...): epoll_ctl(...) failed with error %s\n", strerror(errno));
		return;
	}

	poll_set->size++;
}

void poll_set_remove(struct poll_set* poll_set, int fd) {
	struct epoll_event ev = {.data.fd = fd};
	if (epoll_ctl(poll_set->fd, EPOLL_CTL_DEL, fd, &ev)) {
		printf("[error] poll_set_remove(...): epoll_ctl(...) failed with error %s\n", strerror(errno));
		return;
	}
	poll_set->size--;
}

void poll_set_wait_for_events(struct poll_set* poll_set) {
	int num_events = epoll_wait(poll_set->fd, poll_set->events, poll_set->max_events, -1);
	if (num_events > 0) {
		for (int i = 0; i < num_events; i++) {
			struct epoll_event ev = poll_set->events[i];
			if (ev.events & EPOLLRDHUP) {
				// Client disconnected
				poll_set_remove(poll_set, ev.data.fd);
				poll_set->close_callback(ev.data.fd);
			} else if (ev.events & EPOLLIN) {
				poll_set->data_callback(ev.data.fd);
			} else {
				// ?
			}
		}
	} else {
		printf("[error] poll_set_poll(...): epoll_wait(...) failed with error %s\n", strerror(errno));
	}
}
