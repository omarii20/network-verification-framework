#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define UDP_PORT 9090

int main(void)
{
    // Step 1: Create UDP socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0){
        perror("UDP socket creation failed");
        return EXIT_FAILURE;
    }

    printf("UDP socket created successfully, FD %d\n", server_fd);

    // Step 2: Configure server address
    struct sockaddr_in server_addr = {0};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Step 3: Bind UDP socket
    if (bind(server_fd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("UDP bind failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("UDP socket bound successfully to port %d\n", UDP_PORT);

    char buffer[1024];

    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    printf("UDP server waiting for datagrams...\n");

    while (1){
        client_addr_len = sizeof(client_addr);

        ssize_t bytes_received = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &client_addr_len);

        if (bytes_received < 0){
            perror("UDP recvfrom failed");
            continue;
        }

        buffer[bytes_received] = '\0';

        printf( "Received %zd bytes from %s:%u -> %s\n",bytes_received, inet_ntoa(client_addr.sin_addr), (unsigned)ntohs(client_addr.sin_port), buffer);

        ssize_t bytes_sent = sendto( server_fd, buffer, (size_t)bytes_received,0, (struct sockaddr *)&client_addr, client_addr_len);

        if (bytes_sent < 0){
            perror("UDP sendto failed");
            continue;
        }

        printf("Echoed %zd bytes back to client %s:%u\n", bytes_sent, inet_ntoa(client_addr.sin_addr), (unsigned)ntohs(client_addr.sin_port));
    }

    close(server_fd);

    return EXIT_SUCCESS;
}