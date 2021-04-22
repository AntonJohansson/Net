#include "../include/net.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct binary_string* binary_string_new(size_t size) {
	void* p = malloc(sizeof(struct binary_string) + size);
	if (p == NULL) {
		printf("[error] binary_string_new(...): malloc failed!\n");
		return NULL;
	}

	struct binary_string* bs = p;
	bs->data = (uint8_t*)(bs + 1);
	bs->size = size;
	bs->top = 0;

	return bs;
}

void binary_string_free(struct binary_string* bs) {
	free(bs);
}

void binary_string_append(struct binary_string* bs, const uint8_t* data, size_t size) {
	if (bs->top + size <= bs->size) {
		memcpy(bs->data, data, size);
		bs->top += size;
	} else {
		printf("[error] binary_string_append(...): could not append to string (no space)!\n");
	}
}

void binary_string_encode_string(struct binary_string* bs, const char* str, size_t len) {
	binary_string_encode_int(bs, (uint8_t*)&len, sizeof(len));
	binary_string_append(bs, (uint8_t*)str, len);
}
