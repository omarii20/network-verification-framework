#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "socket_utils.h"
#include "server_config.h"

int set_nonblocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}

int create_server_socket(void){
    // Create TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    printf("Socket created successfully, FD %d\n", sockfd);

    // Set listening socket to non-blocking mode
    if (set_nonblocking(sockfd) == -1) {
        close(sockfd);
        return -1;
    }

    // Allow address reuse
    int option = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    // Configure server address
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to PORT
    if (bind(sockfd, (struct sockaddr *)&server_addr,sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    printf("Socket bound successfully to port %d\n", PORT);

    // Start listening
    if (listen(sockfd, 5) == -1) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}