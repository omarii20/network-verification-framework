#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_EVENTS 100
#define BUFFER_SIZE 1024

// Set a file descriptor to non-blocking mode
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

/*
 * מטפלת באירוע קריאה אחד של Client.
 *
 * מחזירה:
 *  0  -> ה-Client עדיין מחובר
 * -1  -> צריך לסגור ולהסיר את ה-Client
 */

 int handle_client_event(int client_fd){
    char buffer[BUFFER_SIZE];

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received == -1) {
        /*
         * Non-blocking socket:
         * there is currently no more data available to read.
        */
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }

        perror("recv");
        return -1;
    }

    /*
     * recv() returned 0:
     * the client closed the connection.
     */
    if (bytes_received == 0) {
        printf("Client disconnected, FD %d\n", client_fd);
        return -1;
    }

    buffer[bytes_received] = '\0';
    printf("Received %zd bytes from FD %d\n",bytes_received, client_fd);
    printf("Message from FD %d: %s", client_fd, buffer);

    /*
     * Prepare server response.
     */
    const char *response ="Message received successfully\n";
    size_t response_length = strlen(response);
    ssize_t bytes_sent = send(client_fd, response, response_length, 0);

    if (bytes_sent == -1) {
        //The socket is currently not ready for writing.
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("FD %d is not ready for writing yet\n", client_fd);
            return 0;
        }

        perror("send");
        return -1;
    }

    /*
     * On a non-blocking socket send() may send
     * fewer bytes than requested.
     */
    if ((size_t)bytes_sent < response_length) {
        printf("Partial send on FD %d: sent %zd of %zu bytes\n", client_fd, bytes_sent, response_length);
    }

    printf("Sent %zd bytes to client FD %d\n", bytes_sent,client_fd);

    return 0;
}

/*
 * Adds a client socket to the epoll instance.
*/
 int add_client(int epoll_fd, int client_fd){
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = client_fd;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1){
        perror("epoll_ctl: add_client");
        return -1;
    }
    printf("Added client FD %d to epoll\n", client_fd);

    return 0;
 }

/*
 * Removes a client socket from the epoll instance
 * and closes the socket.
*/
 void remove_client(int epoll_fd, int client_fd){
    if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1){
        perror("epoll_ctl: delete_client");
    }
    close(client_fd);
    printf("Removed client FD %d from epoll and closed socket\n", client_fd);
 }

int main(void){

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1){
        perror("socket");
        return 1;
    }

    // Set the socket to non-blocking mode
    if (set_nonblocking(sockfd) == -1) {
        close(sockfd);
        return 1;
    }

    printf("Socket created successfully, FD %d\n", sockfd);

    // Set socket options to allow address reuse
    int option = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1){
        perror("setsockopt");
        close(sockfd);
        return 1;
    }

    // Bind the socket to a specific port (8080)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // חיבור ה-socket לפורט.
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("Socket bound successfully to port %d\n", PORT);

    // Listen for incoming connections
    if (listen(sockfd, 5) == -1){
        perror("listen");
        close(sockfd);
        return 1;
    }

    printf("Server is listening on port %d\n", PORT);

    // Create an epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(sockfd);
        return 1;
    }

    // Add the listening socket to the epoll instance
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sockfd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        perror("epoll_ctl ADD listen socket");
        close(sockfd);
        close(epoll_fd);
        return 1;
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

            if (current_fd == sockfd) {
                /*
                * Listening socket is ready.
                * Accept every connection currently waiting.
                */
                while (1) {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);

                    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

                    if (client_fd == -1) {
                        /*
                        * Non-blocking listening socket:
                        * no more clients are waiting.
                        */
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        }
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
            }else{
                if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    printf("Socket error/hangup on FD %d\n", current_fd);
                    remove_client(epoll_fd, current_fd);
                    continue;
                }

                if (events[i].events & EPOLLIN) {
                    int result = handle_client_event(current_fd);
                    if (result == -1) {
                        remove_client(epoll_fd, current_fd);
                    }
                }
            }
        }
    }

    close(epoll_fd);
    close(sockfd);

    return EXIT_SUCCESS;
}