PROJECT = net
BUILDDIR = build

OUTLIB = $(BUILDDIR)/$(PROJECT).a

SRCS = src/socket.c \
	   src/poll_set.c
TEST_SERVER_SRCS = tests/server/main.c
TEST_CLIENT_SRCS = tests/client/main.c
HDRS = include/net.h

OBJS = $(patsubst %.c, $(BUILDDIR)/%.o, $(SRCS))

FLAGS = -g -c -fPIC -pedantic -Wall -Wextra

all: $(OUTLIB)

$(BUILDDIR):
	mkdir -p $(shell find src -type d | sed -e "s/^/$(BUILDDIR)\//")

$(BUILDDIR)/%.o: %.c
	$(CC) -o $@ $^ $(FLAGS)

$(OUTLIB): $(BUILDDIR) $(OBJS)
	ar rcs $@ $(OBJS)

.PHONY: tests
tests: $(BUILDDIR)/test_server $(BUILDDIR)/test_client
$(BUILDDIR)/test_server: $(TEST_SERVER_SRCS) $(OUTLIB)
	$(CC) -o $@ $^ -L . -l:$(OUTLIB) -lpthread

$(BUILDDIR)/test_client: $(TEST_CLIENT_SRCS) $(OUTLIB)
	$(CC) -o $@ $^ -L . -l:$(OUTLIB) -lpthread

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
