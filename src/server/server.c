#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

int main(void)
{
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
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

        while (1){

            // Receive data from the client
            char buffer[1024];
            ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_received == -1){
                perror("send");
                break;
            }

            if (bytes_received == 0){
                printf("Client disconnected, FD %d\n", client_fd);
                break;
            }

            buffer[bytes_received] = '\0';

            printf("Received %zd bytes\n", bytes_received);
            printf("Message: %s\n", buffer);

            // Send a response back to the client
            const char *response = "Message received successfully\n";
            size_t response_length = strlen(response);

            ssize_t bytes_sent = send(client_fd, response, response_length, 0);

            if (bytes_sent == -1){
                perror("send");
            }else{
                printf("Sent %zd bytes to client\n", bytes_sent);

                if ((size_t)bytes_sent < response_length){
                    printf("Warning: only part of the response was sent\n");
                }
            }
        }
        close(client_fd);
    }

    return 0;
}