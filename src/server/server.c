#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "socket_utils.h"
#include "client_handler.h"
#include "server_config.h"
#include "epoll_manager.h"

int main(void)
{
    int sockfd = create_server_socket();

    if (sockfd == -1) {
        return EXIT_FAILURE;
    }

    printf( "Server is listening on port %d, FD %d\n", PORT, sockfd);

    int epoll_fd = create_epoll_instance(sockfd);

    if (epoll_fd == -1) {
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Main event loop
    struct epoll_event events[MAX_EVENTS];

    while (1) {
        printf("Waiting for events...\n");

        int ready_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        if (ready_count == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < ready_count; i++) {
            int current_fd = events[i].data.fd;

            /*
             * Listening socket is ready.
             * Accept all connections currently waiting.
             */
            if (current_fd == sockfd) {
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);

                    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

                    if (client_fd == -1) {
                        /*
                         * Non-blocking listening socket:
                         * no more connections are waiting.
                         */
                        if (errno == EAGAIN || errno == EWOULDBLOCK) { break ;}

                        perror("accept");
                        break;
                    }

                    printf("Client connected successfully, FD %d\n", client_fd);

                    if (set_nonblocking(client_fd) == -1) {
                        close(client_fd);
                        continue;
                    }

                    if (add_client(epoll_fd, client_fd) == -1) {
                        close(client_fd);
                        continue;
                    }
                }

                continue;
            }

            /*
             * Client socket error or disconnect.
             */
            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                printf("Socket error/hangup on FD %d\n", current_fd);

                remove_client(epoll_fd, current_fd);
                continue;
            }

            /*
             * Client socket is ready for reading.
             */
            if (events[i].events & EPOLLIN) {
                int result = handle_client_event(current_fd);

                if (result == -1) {
                    remove_client(epoll_fd, current_fd);
                }
            }
        }
    }

    close(epoll_fd);
    close(sockfd);

    return EXIT_SUCCESS;
}