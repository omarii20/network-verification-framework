#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

int add_client(int epoll_fd, int client_fd);
void remove_client(int epoll_fd, int client_fd);
int create_epoll_instance(int listen_fd);

#endif