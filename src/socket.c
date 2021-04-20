#include "../include/net.h"

// cstdlib
#include <stdio.h>

// Unix
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>

Socket socket_open(Socket_Mode mode) {
	return (Socket){};
}

void socket_close(Socket* socket) {
	close(socket->fd);
}

void socket_shutdown(Socket* socket) {
	shutdown(socket->fd, SHUT_RDWR);
}

void socket_set_blocking(Socket* socket, const bool state) {
	if (state) {
		// Unset O_NONBLOCK flag
		int opts = fcntl(socket->fd, F_GETFL);
		opts = opts & (~O_NONBLOCK);
		fcntl(socket->fd, F_SETFL, opts);
	} else {
		// Set O_NONBLOCK flag
		fcntl(socket->fd, F_SETFL, O_NONBLOCK);
	}
}

size_t socket_send(Socket* socket, const uint8_t* data, const size_t size) {
	ssize_t bytes_sent = send(socket->fd, data, size, 0);
	if (send(socket->fd, data, size, 0) == -1) {
		if (errno = EAGAIN || errno == EWOULDBLOCK) {
			// ?
		} else if (errno == ECONNRESET) {
			// Connection closed by client
		} else {
			// Send failed
			printf("[error] Socket %d -- send(...) failed with error: %s\n", socket->fd, strerror(errno));
		}
	}

	return (size_t)bytes_sent;
}

void socket_send_all(Socket* socket, const uint8_t* data, const size_t size) {
	size_t total_bytes_sent = 0;
	while (total_bytes_sent < size) {
		size_t num_bytes_sent = send(socket->fd, data + total_bytes_sent, size - total_bytes_sent, 0);
		total_bytes_sent += num_bytes_sent;
	}
}

size_t socket_recv(Socket* socket, uint8_t* buf, const size_t buf_size) {
	ssize_t bytes_received = recv(socket->fd, buf, buf_size-1, 0);
	if (bytes_received <= 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// No data to be received
		} else if (errno == ECONNRESET || bytes_received == 0) {
			// Connection closed by client
		} else {
			printf("[error] Socket %d -- recv(...) failed with error %s\n", socket->fd, strerror(errno));
		}

		bytes_received = 0;
	}

	return (size_t)bytes_received;
}


























#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

Socket server_bind_tcp(const char* port) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	struct addrinfo* info;
	int err = getaddrinfo(NULL, port, &hints, &info);
	if (err != 0) {
		printf("[error] server_bind_tcp(...): getaddrinfo(...) failed with error %s\n", gai_strerror(err));
		return (Socket){};
	}

	int sock = -1;
	const int yes = 1;
	for (struct addrinfo* p = info; p != NULL; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock == -1) {
			printf("[error] server_bind_tcp(...): socket(...) failed with error %s\n", strerror(errno));
			continue;
		}

		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			printf("[error] server_bind_tcp(...): setsockopt(...) failed with error %s\n", strerror(errno));
			return (Socket){};
		}

		if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
			close(sock);
			printf("[error] server_bind_tcp(...): bind(...) failed with error %s\n", strerror(errno));
			continue;
		}

		// If we get here the socket is up and bound
		break;
	}

	freeaddrinfo(info);

	if (sock == -1) {
		printf("[error] server_bind_tcp(...): Failed to bind socket\n");
		return (Socket){};
	}

	return (Socket){.fd = sock};
}

void server_listen(Socket* socket, uint32_t backlog) {
	if (listen(socket->fd, backlog) == -1) {
		printf("[error] server_listen(...): listen(...) failed with error %s\n",  strerror(errno));
	}
}

Socket server_accept(Socket* socket) {
	struct sockaddr_storage their_addr;
	socklen_t sin_size = sizeof(their_addr);

	int sock = accept(socket->fd, (struct sockaddr*)&their_addr, &sin_size);
	if (sock == -1) {
		printf("[error] server_accept(...): accept(...) failed with error %s\n",  strerror(errno));
		return (Socket){};
	}

	return (Socket){.fd = sock};
}

Socket client_bind_tcp(const char* ip, const char* port) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* info;
	int err = getaddrinfo(ip, port, &hints, &info);
	if (err != 0) {
		printf("[error] client_bind_tcp(...): getaddrinfo(...) failed with error %s\n", gai_strerror(err));
		return (Socket){};
	}

	int sock = -1;
	struct addrinfo* p = NULL;
	for (p = info; p != NULL; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock == -1) {
			printf("[error] client_bind_tcp(...): socket(...) failed with error %s\n", strerror(errno));
			continue;
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
			close(sock);
			printf("[error] client_bind_tcp(...): connect(...) failed with error %s\n", strerror(errno));
			continue;
		}

		// If we get here the socket is up and bound
		break;
	}

	freeaddrinfo(info);

	if (sock == -1) {
		printf("[error] server_bind_tcp(...): Failed to bind socket\n");
		return (Socket){};
	} else if (p == NULL) {
		printf("[error] server_bind_tcp(...): Failed to connect to server\n");
		return (Socket){};
	}

	return (Socket){.fd = sock};
}
