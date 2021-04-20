#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


// Socket
enum Socket_Mode {
	SOCKET_MODE_TCP = 0,
	SOCKET_MODE_UDP = 1,
};
typedef enum Socket_Mode Socket_Mode;

struct Socket {
	int fd;
	Socket_Mode mode;
};
typedef struct Socket Socket;

Socket socket_open(Socket_Mode mode);
void socket_close(Socket* socket);
void socket_shutdown(Socket* socket);

void socket_set_blocking(Socket* socket, const bool state);

size_t socket_send(Socket* socket, const uint8_t* data, const size_t size);
void socket_send_all(Socket* socket, const uint8_t* data, const size_t size);
size_t socket_recv(Socket* socket, uint8_t* buf, const size_t buf_size);

// Server/client stuff
Socket server_bind_tcp(const char* port);
void server_listen(Socket* socket, uint32_t backlog);
Socket server_accept(Socket* socket);
Socket client_bind_tcp(const char* ip, const char* port);


// Poll set

typedef void Poll_Set_Callback_Func(int);

struct epoll_event;

struct Poll_Set {
	struct epoll_event* events;
	Poll_Set_Callback_Func* data_callback;
	Poll_Set_Callback_Func* close_callback;
	int size;
	int max_events;
	int fd;
};
typedef struct Poll_Set Poll_Set;

Poll_Set* poll_set_new(int max_events, Poll_Set_Callback_Func* data_callback, Poll_Set_Callback_Func* close_callback);
void poll_set_free(Poll_Set* poll_set);
void poll_set_add(Poll_Set* poll_set, int fd);
void poll_set_poll(Poll_Set* poll_set);
