#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "epoll_manager.h"

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

// Create an epoll instance
int create_epoll_instance(int listen_fd){
    int epoll_fd = epoll_create1(0);

    if (epoll_fd == -1) {
        perror("epoll_create1");
        return -1;
    }

    // Add the listening socket to the epoll instance
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = listen_fd;

    if ( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
        perror("epoll_ctl ADD listen socket");
        close(epoll_fd);
        return -1;
    }

    return epoll_fd;
}