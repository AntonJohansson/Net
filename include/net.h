#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum socket_state {
	SOCKET_INVALID = -1
};


void socket_close(int socket);
void socket_shutdown(int socket);

void socket_set_blocking(int socket, const bool state);

size_t socket_send(int socket, const uint8_t* data, const size_t size);
void   socket_send_all(int socket, const uint8_t* data, const size_t size);
size_t socket_recv(int socket, uint8_t* buf, const size_t buf_size);

// Server/client stuff
int server_bind_tcp(const char* port);
void server_listen(int socket, uint32_t backlog);
int server_accept(int socket);
int client_bind_tcp(const char* ip, const char* port);


// Poll set
typedef void poll_set_callback_func(int);
struct epoll_event;

struct poll_set {
	struct epoll_event* events;
	poll_set_callback_func* data_callback;
	poll_set_callback_func* close_callback;
	int size;
	int max_events;
	int fd;
};

struct poll_set* poll_set_new(int max_events, poll_set_callback_func* data_callback, poll_set_callback_func* close_callback);
void poll_set_free(struct poll_set* set);
void poll_set_add(struct poll_set* set, int fd);
void poll_set_wait_for_events(struct poll_set* set);

// Binary Encoding
struct binary_string {
	uint8_t* data;
	size_t	 size;
	size_t	 top;
};

struct binary_string* 	binary_string_new(size_t size);
void 					binary_string_free(struct binary_string* bs);
void 					binary_string_append(struct binary_string* bs, const uint8_t* data, size_t size);

void binary_string_encode_string(struct binary_string* bs, const char* str, size_t len);
void binary_string_encode_int(struct binary_string* bs, uint8_t* data, size_t len);
