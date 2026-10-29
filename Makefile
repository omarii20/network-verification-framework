
CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Iinclude/tcp

# TCP Server
TCP_SRC = \
	src/tcp/server.c \
	src/tcp/socket_utils.c \
	src/tcp/client_handler.c \
	src/tcp/epoll_manager.c

TCP_BIN = tcp_server

# UDP Server
UDP_SRC = src/udp/udp_server.c

UDP_BIN = udp_server

# Build both servers
all: $(TCP_BIN) $(UDP_BIN)

$(TCP_BIN): $(TCP_SRC)
	$(CC) $(CFLAGS) $(TCP_SRC) -o $(TCP_BIN)

$(UDP_BIN): $(UDP_SRC)
	$(CC) $(CFLAGS) $(UDP_SRC) -o $(UDP_BIN)

# Run TCP Server
run: $(TCP_BIN)
	./$(TCP_BIN)

# Run UDP Server
run-udp: $(UDP_BIN)
	./$(UDP_BIN)

# Run Python tests
test:
	pytest -v tests/python/

# Remove binaries
clean:
	rm -f $(TCP_BIN) $(UDP_BIN)

.PHONY: all run run-udp test clean
