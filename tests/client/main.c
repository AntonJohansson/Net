#include "../../include/net.h"
//#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define BUFLEN 64

int main() {
	Socket s = client_bind_tcp("127.0.0.1", "4040");
	char buf[BUFLEN];
	while (true) {
		memset(buf, 0, BUFLEN);
		fgets(buf, BUFLEN, stdin);
		buf[BUFLEN-1] = 0;
		socket_send_all(&s, buf, BUFLEN);
	}
	socket_close(&s);
	return 0;
}
