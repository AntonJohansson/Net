#include "../../include/net.h"
//#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

int server;
struct poll_set* set;
char buf[16];

void data_callback(int fd) {
	if (fd == server.fd) {
		Socket client = server_accept(&server);
		socket_set_blocking(&client, false);
		poll_set_add(set, client.fd);
		printf("Connection on %d\n", client.fd);

		return;
	}

	size_t n;
	do {
		n = socket_recv(fd, buf, 16);
		buf[n] = 0;
		printf("recv'd %d bytes %s from %d\n", n, buf, fd);
	} while (n > 0);
}

void close_callback(int fd) {
	printf("Lost connection on %d\n", fd);
}





















int main() {
	server = server_bind_tcp("4040");
	socket_set_blocking(server, false);

	set = poll_set_new(16, data_callback, close_callback);
	poll_set_add(set, server.fd);

	server_listen(server, 16);

	while (true) {
		poll_set_wait_for_events(set);
	}

	poll_set_free(set);

	socket_close(server);
	return 0;
}
