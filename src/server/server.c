#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>


void *handle_client(void *arg){
    
    int client_fd = *(int *)arg;
    free(arg);

    printf("Thread started for client FD %d\n", client_fd);

    while (1){
        char buffer[1024];

        ssize_t bytes_received = recv( client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == -1){
            perror("recv");
            break;
        }

        if (bytes_received == 0){
            printf("Client disconnected, FD %d\n", client_fd);
            break;
        }

        buffer[bytes_received] = '\0';

        printf("Received %zd bytes from FD %d\n", bytes_received, client_fd);
        printf("Message from FD %d: %s", client_fd, buffer);

        const char *response = "Message received successfully\n";
        size_t response_length = strlen(response);

        ssize_t bytes_sent = send( client_fd, response, response_length, 0);

        if (bytes_sent == -1){
            perror("send");
            break;
        }

        printf("Sent %zd bytes to client FD %d\n", bytes_sent, client_fd);
    }

    close(client_fd);

    printf("Closed client socket FD %d\n", client_fd);

    return NULL;
}

int main(void){

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1){
        perror("socket");
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
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("Socket bound successfully to port 8080\n");

    // Listen for incoming connections
    if (listen(sockfd, 5) == -1){
        perror("listen");
        close(sockfd);
        return 1;
    }

    printf("Server is listening on port 8080\n");

    while (1){
        // Accept an incoming connection
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        printf("Waiting for incoming connections...\n");

        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd == -1){
            perror("accept");
            continue;
        }

        printf("Client connected successfully, FD %d\n", client_fd);
        printf("Listening socket FD: %d\n", sockfd);
        printf("Client socket FD: %d\n", client_fd);

        int *client_fd_ptr = malloc(sizeof(int));

        if (client_fd_ptr == NULL){
            perror("malloc");
            close(client_fd);
            continue;
        }

        *client_fd_ptr = client_fd;
        pthread_t thread_id;

        int result = pthread_create(&thread_id, NULL, handle_client, client_fd_ptr);

        if (result != 0){
            fprintf(stderr, "pthread create failed: %s\n", strerror(result));

            free(client_fd_ptr);
            close(client_fd);
            continue;
        }

        result = pthread_detach(thread_id);

        if (result != 0){
            fprintf(stderr, "pthread_detach failed: %s\n", strerror(result));
        }
    }    

    return 0;
}