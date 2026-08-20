CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Iinclude

SERVER_SRC = \
	src/server/server.c \
	src/server/socket_utils.c \
	src/server/client_handler.c \
	src/server/epoll_manager.c

SERVER_BIN = server

all: $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER_BIN)

run: $(SERVER_BIN)
	./$(SERVER_BIN)

clean:
	rm -f $(SERVER_BIN)

.PHONY: all run clean